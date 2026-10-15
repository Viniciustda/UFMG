"""Blend do CE fine-tuned (OOF) com rrf/ce zero-shot: grid no train + submissão.

Uso: python src/eval_ceft_blend.py [--out runs/sub6_ceft_blend.csv]
"""
from __future__ import annotations

import argparse

import numpy as np
import pandas as pd

from common import CACHE, load_qrels, write_run
from evaluate import ndcg_run
from rank import split_run

ap = argparse.ArgumentParser()
ap.add_argument("--out", default=None)
args = ap.parse_args()

qrels = load_qrels()
ceft = pd.read_csv(CACHE / "ce_ft_scores.csv.gz", dtype={"QueryId": str, "EntityId": str})
pool = pd.read_csv(CACHE / "pool_fused.csv.gz", dtype={"QueryId": str, "EntityId": str})
ce = pd.read_csv(CACHE / "ce_scores.csv.gz", dtype={"QueryId": str, "EntityId": str})
ce = ce[["QueryId", "EntityId", "ce_score"]].rename(columns={"ce_score": "ce"})

df = ceft[["QueryId", "EntityId", "row", "ce_score"]].rename(columns={"ce_score": "ceft"})
df = df.merge(pool[["QueryId", "EntityId", "rrf"]], on=["QueryId", "EntityId"], how="left")
df = df.merge(ce, on=["QueryId", "EntityId"], how="left")


def z(col):
    def f(g):
        s = g.std()
        return (g - g.mean()) / s if s > 1e-9 else g * 0
    return df.groupby("QueryId")[col].transform(f)


df["rrf_z"] = z("rrf")
df["ce_z"] = z("ce")
df["ceft_z"] = z("ceft")
tr = df[df.QueryId.isin(qrels)]


def evalmix(w_rrf, w_ce, w_ceft, frame):
    frame = frame.copy()
    frame["mix"] = (w_rrf * frame["rrf_z"].fillna(0) + w_ce * frame["ce_z"].fillna(0)
                    + w_ceft * frame["ceft_z"].fillna(0))
    return {q: g.sort_values("mix", ascending=False)["EntityId"].tolist()[:100]
            for q, g in frame.groupby("QueryId")}


best = (-1, None)
for w_ce in np.arange(0.0, 0.65, 0.1):
    for w_ceft in np.arange(0.0, 0.75, 0.05):
        w_rrf = 1.0 - w_ce - w_ceft
        if w_rrf < 0.05:
            continue
        s = ndcg_run(evalmix(w_rrf, w_ce, w_ceft, tr), qrels)
        if s > best[0]:
            best = (s, (round(w_rrf, 2), round(w_ce, 2), round(w_ceft, 2)))
            print(f"rrf={best[1][0]} ce={best[1][1]} ceft={best[1][2]}: {s:.4f}")
print(f"MELHOR: {best[1]} -> {best[0]:.4f}")

if args.out:
    w = best[1]
    run = evalmix(*w, df)
    train, test = split_run(run)
    write_run(test, args.out)
    write_run(train, str(args.out).replace(".csv", "_train.csv"))
    print(f"submissão: {args.out} (pesos rrf/ce/ceft = {w})")
