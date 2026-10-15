"""Rerank do pool com cross-encoder (zero-shot ou fine-tuned) — GPU.

Pontua (query, doc) no top-K do pool, mistura com o sinal lexical via z-norm
(α escolhido no train) e gera a submissão. Checkpoint a cada ~12k pares.

Uso:
  python src/rerank_ce.py --pool cache/pool_lex.csv.gz --depth 1000 \
      --scores cache/ce_scores.csv.gz --out runs/sub4_ce.csv
  (se --scores já existe, pula o scoring e só faz o blend/submissão)
Opções: --model BAAI/bge-reranker-base | caminho de checkpoint fine-tuned
        --col score (coluna do pool usada como sinal lexical no blend)
"""
from __future__ import annotations

import argparse
import csv
import gzip
import time
from pathlib import Path

import numpy as np
import pandas as pd

from common import CACHE, CORPUS, all_queries, load_qrels, write_run
from evaluate import ndcg_run
from rank import split_run

ALPHAS = [round(0.1 * a, 1) for a in range(11)]


def fetch_texts(rows: set[int]) -> dict[int, str]:
    """Uma passada sequencial no corpus; retorna row -> texto p/ o CE."""
    import orjson

    out: dict[int, str] = {}
    t0 = time.time()
    with open(CORPUS, "rb") as f:
        for i, line in enumerate(f):
            if i in rows:
                d = orjson.loads(line)
                kw = d.get("keywords") or []
                if isinstance(kw, list):
                    kw = ", ".join(str(x) for x in kw)
                out[i] = f"{d.get('title') or ''}. {kw}. {str(d.get('text') or '')[:1200]}"
    print(f"textos: {len(out)} docs ({time.time()-t0:.0f}s)")
    return out


def ce_score(sub: pd.DataFrame, texts: dict[int, str], queries: dict[str, str],
             model_name: str, batch: int, maxlen: int, ckpt: Path) -> np.ndarray:
    import torch
    from transformers import AutoModelForSequenceClassification, AutoTokenizer

    tok = AutoTokenizer.from_pretrained(model_name)
    model = AutoModelForSequenceClassification.from_pretrained(
        model_name, torch_dtype=torch.float16)
    model.eval().to("cuda")

    scores = np.full(len(sub), np.nan, dtype=np.float32)
    if ckpt.exists():
        old = np.load(ckpt)
        if len(old) == len(sub):
            scores = old
            print(f"retomando: {np.isfinite(scores).sum()}/{len(sub)} já pontuados")
    todo = np.where(~np.isfinite(scores))[0]
    qtexts = sub["QueryId"].map(queries).to_numpy()
    rows_np = sub["row"].to_numpy()
    t0 = time.time()
    with torch.no_grad():
        for bi in range(0, len(todo), batch):
            idxs = todo[bi: bi + batch]
            pairs_q = [qtexts[i] for i in idxs]
            pairs_d = [texts.get(int(rows_np[i]), "") for i in idxs]
            enc = tok(pairs_q, pairs_d, padding=True, truncation=True,
                      max_length=maxlen, return_tensors="pt").to("cuda")
            out = model(**enc).logits.squeeze(-1).float().cpu().numpy()
            scores[idxs] = out
            if bi % (batch * 200) == 0:
                rate = (bi + len(idxs)) / max(time.time() - t0, 1e-9)
                eta = (len(todo) - bi) / max(rate, 1)
                print(f"{np.isfinite(scores).sum()}/{len(sub)}  {rate:.0f} pares/s  "
                      f"ETA {eta/60:.0f}min", flush=True)
                np.save(ckpt, scores)
    np.save(ckpt, scores)
    return scores


def znorm(g: pd.Series) -> pd.Series:
    s = g.std()
    return (g - g.mean()) / s if s > 1e-9 else g * 0.0


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--pool", default=str(CACHE / "pool_lex.csv.gz"))
    ap.add_argument("--col", default="score", help="coluna lexical p/ blend")
    ap.add_argument("--depth", type=int, default=1000)
    ap.add_argument("--model", default="BAAI/bge-reranker-base")
    ap.add_argument("--batch", type=int, default=64)
    ap.add_argument("--maxlen", type=int, default=288)
    ap.add_argument("--scores", default=str(CACHE / "ce_scores.csv.gz"))
    ap.add_argument("--out", default=None)
    ap.add_argument("--alpha", type=float, default=None)
    args = ap.parse_args()

    queries = {qid: q for qid, q in all_queries()}
    qrels = load_qrels()

    scores_path = Path(args.scores)
    if scores_path.exists():
        sub = pd.read_csv(scores_path, dtype={"QueryId": str, "EntityId": str})
        if "lex.1" in sub.columns:  # legado: rename duplicado em execução antiga
            sub = sub.rename(columns={"lex.1": "rrf"})
        print(f"scores carregados: {len(sub)} pares")
    else:
        pool = pd.read_csv(args.pool, dtype={"QueryId": str, "EntityId": str})
        lexcol = args.col if args.col in pool.columns else "score"
        pool = pool.sort_values(["QueryId", lexcol], ascending=[True, False])
        sub = pool.groupby("QueryId").head(args.depth).reset_index(drop=True)
        print(f"{len(sub)} pares a pontuar (depth {args.depth})")
        texts = fetch_texts(set(sub["row"].astype(int)))
        ckpt = scores_path.with_suffix(".ckpt.npy")
        sub["ce_score"] = ce_score(sub, texts, queries, args.model,
                                   args.batch, args.maxlen, ckpt)
        sub.to_csv(scores_path, index=False, compression="gzip")
        ckpt.unlink(missing_ok=True)
        print(f"scores salvos -> {scores_path}")

    lexcol = args.col if args.col in sub.columns else ("lex" if "lex" in sub.columns else "score")
    sub["lex_z"] = sub.groupby("QueryId")[lexcol].transform(znorm)
    sub["ce_z"] = sub.groupby("QueryId")["ce_score"].transform(znorm)

    def run_at(a: float) -> dict[str, list[str]]:
        sub["mix"] = (1 - a) * sub["lex_z"] + a * sub["ce_z"]
        out = {}
        for qid, g in sub.groupby("QueryId"):
            out[qid] = g.sort_values("mix", ascending=False)["EntityId"].tolist()[:100]
        return out

    if args.alpha is None:
        best = (-1.0, 0.0)
        for a in ALPHAS:
            train, _ = split_run(run_at(a))
            s = ndcg_run(train, qrels)
            print(f"alpha={a}: nDCG@100 train = {s:.4f}", flush=True)
            if s > best[0]:
                best = (s, a)
        alpha = best[1]
        print(f"MELHOR: alpha={alpha} ({best[0]:.4f})")
    else:
        alpha = args.alpha
        train, _ = split_run(run_at(alpha))
        print(f"alpha={alpha}: nDCG@100 train = {ndcg_run(train, qrels):.4f}")

    if args.out:
        _, test = split_run(run_at(alpha))
        write_run(test, args.out)
        train, _ = split_run(run_at(alpha))
        write_run(train, str(args.out).replace(".csv", "_train.csv"))
        print(f"submissão: {args.out} (alpha={alpha})")


if __name__ == "__main__":
    main()
