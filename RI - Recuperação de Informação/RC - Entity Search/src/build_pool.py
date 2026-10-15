"""Funde pool lexical (BM25F+RM3) com 1+ rankings densos via RRF.

Saída: cache/pool_fused.csv.gz (QueryId, EntityId, row, lex, dense[, dense2], rrf)
Também avalia RRF puro no train (candidato a submissão intermediária).

Uso:
  python src/build_pool.py --depth 2000                       # lex + bge-small
  python src/build_pool.py --dense cache/dense_top.csv.gz \\
      --dense2 cache/dense_top_base.csv.gz --depth 2000       # lex + small + base
"""
from __future__ import annotations

import argparse

import numpy as np
import pandas as pd

from common import CACHE, load_qrels, write_run
from evaluate import ndcg_run
from rank import split_run


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--lex", default=str(CACHE / "pool_lex.csv.gz"))
    ap.add_argument("--dense", default=str(CACHE / "dense_top.csv.gz"))
    ap.add_argument("--dense2", default=None, help="segundo ranking denso (ex: bge-base)")
    ap.add_argument("--depth", type=int, default=2000)
    ap.add_argument("--rrf-k", type=int, default=60)
    ap.add_argument("--out", default=str(CACHE / "pool_fused.csv.gz"))
    ap.add_argument("--sub", default=None, help="grava submissão RRF pura")
    args = ap.parse_args()
    K = args.rrf_k

    lex = pd.read_csv(args.lex, dtype={"QueryId": str, "EntityId": str})
    lex = lex.rename(columns={"score": "lex"})
    lex["r_lex"] = lex.groupby("QueryId")["lex"].rank(ascending=False, method="first")

    den = pd.read_csv(args.dense, dtype={"QueryId": str, "EntityId": str})
    den = den.rename(columns={"score": "dense"})
    den["r_den"] = den.groupby("QueryId")["dense"].rank(ascending=False, method="first")

    m = pd.merge(lex, den, on=["QueryId", "EntityId", "row"], how="outer")
    m["rrf"] = 1.0 / (K + m["r_lex"].fillna(1e9)) + 1.0 / (K + m["r_den"].fillna(1e9))
    keep = ["QueryId", "EntityId", "row", "lex", "dense"]

    if args.dense2:
        den2 = pd.read_csv(args.dense2, dtype={"QueryId": str, "EntityId": str})
        den2 = den2.rename(columns={"score": "dense2"})
        den2["r_den2"] = den2.groupby("QueryId")["dense2"].rank(ascending=False, method="first")
        m = pd.merge(m, den2, on=["QueryId", "EntityId", "row"], how="outer")
        m["rrf"] = m["rrf"].fillna(0.0) + 1.0 / (K + m["r_den2"].fillna(1e9))
        keep.append("dense2")

    keep.append("rrf")
    m = m.sort_values(["QueryId", "rrf"], ascending=[True, False])
    m = m.groupby("QueryId").head(args.depth).reset_index(drop=True)
    m[keep].to_csv(args.out, index=False, compression="gzip")
    print(f"pool fundido: {len(m)} pares -> {args.out}  (fontes: {keep[3:-1]})")

    qrels = load_qrels()
    run = {qid: g["EntityId"].tolist()[:100] for qid, g in m.groupby("QueryId")}
    train, test = split_run(run)
    print(f"RRF nDCG@100 train = {ndcg_run(train, qrels):.4f}")
    pos = {qid: set(g["EntityId"]) for qid, g in m.groupby("QueryId")}
    m_sorted = {qid: g.sort_values("rrf", ascending=False)["EntityId"].tolist()
                for qid, g in m.groupby("QueryId")}
    for k in (100, 500, 1000, 2000):
        vals = []
        for qid, judged in qrels.items():
            rel = {d for d, r in judged.items() if r > 0}
            if not rel:
                continue
            got = set(m_sorted.get(qid, [])[:k])
            vals.append(len(rel & got) / len(rel))
        print(f"recall pool fundido @{k}: {np.mean(vals):.4f}")
    if args.sub:
        write_run(test, args.sub)
        print(f"submissão RRF: {args.sub}")


if __name__ == "__main__":
    main()
