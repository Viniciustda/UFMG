"""Codifica as 4,6M entidades com um bi-encoder (GPU) → memmap fp16.

Texto do doc: "title. keywords. text[:600]". Retomável por blocos.

Uso: python src/dense_encode.py [--model BAAI/bge-small-en-v1.5] [--batch 256]
"""
from __future__ import annotations

import argparse
import json
import time
from pathlib import Path

import numpy as np
import orjson

from common import CACHE, CORPUS

BLOCK = 100_000  # docs por bloco (checkpoint)


def doc_text(doc: dict) -> str:
    kw = doc.get("keywords") or []
    if isinstance(kw, list):
        kw = ", ".join(str(x) for x in kw)
    return f"{doc.get('title') or ''}. {kw}. {str(doc.get('text') or '')[:600]}"


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--model", default="BAAI/bge-small-en-v1.5")
    ap.add_argument("--batch", type=int, default=256)
    ap.add_argument("--maxlen", type=int, default=192)
    ap.add_argument("--suffix", default="", help="sufixo dos arquivos de saída (ex: _base)")
    args = ap.parse_args()

    import torch
    from sentence_transformers import SentenceTransformer

    stats = json.loads((CACHE / "stats.json").read_text(encoding="utf-8"))
    N = stats["N"]
    model = SentenceTransformer(args.model, device="cuda")
    model.max_seq_length = args.maxlen
    model.half()
    dim = model.get_sentence_embedding_dimension()

    emb_path = CACHE / f"dense_emb{args.suffix}.f16"
    meta_path = CACHE / f"dense_emb{args.suffix}.json"
    done = 0
    if meta_path.exists():
        m = json.loads(meta_path.read_text(encoding="utf-8"))
        if m["model"] == args.model and m["N"] == N:
            done = m["done"]
    emb = np.memmap(emb_path, dtype=np.float16, mode="r+" if done else "w+", shape=(N, dim))

    t0 = time.time()
    texts: list[str] = []
    idx = 0
    with open(CORPUS, "rb") as f:
        for line in f:
            if idx >= done:
                texts.append(doc_text(orjson.loads(line)))
                if len(texts) == BLOCK:
                    vec = model.encode(texts, batch_size=args.batch, show_progress_bar=False,
                                       normalize_embeddings=True)
                    emb[idx + 1 - BLOCK: idx + 1] = vec.astype(np.float16)
                    emb.flush()
                    meta_path.write_text(json.dumps(dict(model=args.model, N=N, dim=dim,
                                                         done=idx + 1)), encoding="utf-8")
                    rate = (idx + 1 - done) / (time.time() - t0)
                    eta = (N - idx - 1) / max(rate, 1)
                    print(f"{idx+1}/{N}  {rate:.0f} docs/s  ETA {eta/60:.0f}min", flush=True)
                    texts = []
            idx += 1
    if texts:
        vec = model.encode(texts, batch_size=args.batch, show_progress_bar=False,
                           normalize_embeddings=True)
        emb[idx - len(texts): idx] = vec.astype(np.float16)
        emb.flush()
    meta_path.write_text(json.dumps(dict(model=args.model, N=N, dim=dim, done=N)),
                         encoding="utf-8")
    print(f"ok: {N} docs em {(time.time()-t0)/60:.0f}min -> {emb_path}")


if __name__ == "__main__":
    main()
