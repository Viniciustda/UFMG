"""Busca densa: matmul em blocos na GPU sobre o memmap → top-K por query.

Saída: cache/dense_top.csv.gz (QueryId, EntityId, row, score) + recall no train.

Uso: python src/dense_search.py [--k 1000]
"""
from __future__ import annotations

import argparse
import csv
import gzip
import json

import numpy as np

from common import CACHE, all_queries, load_qrels

QUERY_PREFIX = "Represent this sentence for searching relevant passages: "
CHUNK = 500_000


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--k", type=int, default=1000)
    ap.add_argument("--batch", type=int, default=64)
    ap.add_argument("--suffix", default="", help="sufixo dos arquivos (ex: _base)")
    args = ap.parse_args()

    import torch
    from sentence_transformers import SentenceTransformer

    meta = json.loads((CACHE / f"dense_emb{args.suffix}.json").read_text(encoding="utf-8"))
    assert meta["done"] == meta["N"], f"encoding incompleto: {meta['done']}/{meta['N']}"
    N, dim = meta["N"], meta["dim"]
    emb = np.memmap(CACHE / f"dense_emb{args.suffix}.f16", dtype=np.float16, mode="r",
                    shape=(N, dim))
    ids = np.load(CACHE / "ids.npy")

    queries = all_queries()
    model = SentenceTransformer(meta["model"], device="cuda")
    model.half()
    prefix = QUERY_PREFIX if "bge" in meta["model"].lower() else ""
    qv = model.encode([prefix + q for _, q in queries], batch_size=args.batch,
                      normalize_embeddings=True)
    Q = torch.tensor(qv, dtype=torch.float16, device="cuda")          # (nq, dim)

    nq = len(queries)
    top_s = torch.full((nq, args.k), -1e4, dtype=torch.float32, device="cuda")
    top_i = torch.zeros((nq, args.k), dtype=torch.int64, device="cuda")
    for start in range(0, N, CHUNK):
        end = min(start + CHUNK, N)
        block = torch.from_numpy(np.ascontiguousarray(emb[start:end])).cuda()
        sims = (Q @ block.T).float()                                   # (nq, chunk)
        s, i = torch.topk(sims, k=min(args.k, end - start), dim=1)
        cat_s = torch.cat([top_s, s], dim=1)
        cat_i = torch.cat([top_i, i + start], dim=1)
        s2, pos = torch.topk(cat_s, k=args.k, dim=1)
        top_s = s2
        top_i = torch.gather(cat_i, 1, pos)
        del block, sims
        print(f"{end}/{N}", flush=True)
    top_s = top_s.cpu().numpy()
    top_i = top_i.cpu().numpy()

    out = CACHE / f"dense_top{args.suffix}.csv.gz"
    with gzip.open(out, "wt", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["QueryId", "EntityId", "row", "score"])
        for qi, (qid, _) in enumerate(queries):
            for j in range(args.k):
                w.writerow([qid, ids[top_i[qi, j]].decode(), int(top_i[qi, j]),
                            f"{float(top_s[qi, j]):.5f}"])
    print(f"ok -> {out}")

    qrels = load_qrels()
    qpos = {qid: qi for qi, (qid, _) in enumerate(queries)}
    for k in (100, 500, 1000):
        vals = []
        for qid, judged in qrels.items():
            rel = {d for d, r in judged.items() if r > 0}
            if not rel:
                continue
            got = {ids[top_i[qpos[qid], j]].decode() for j in range(min(k, args.k))}
            vals.append(len(rel & got) / len(rel))
        print(f"recall denso @{k}: {np.mean(vals):.4f}")


if __name__ == "__main__":
    main()
