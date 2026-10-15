"""Ensemble de submissões por fusão de rankings (RRF) ou Borda.

Combina vários runs (csv QueryId,EntityId em ordem) num só. Avalia no train.
Útil no fim: blend dos melhores LTRs costuma render robustez (menor variância
entre folds/leaderboard) sem custo de modelo novo.

Uso:
  python src/ensemble_runs.py runs/sub9_ltr_v2.csv runs/sub10_ltr_ft2.csv \\
      --weights 1,1 --k 60 --out runs/sub_ens.csv
"""
from __future__ import annotations

import argparse

from common import load_qrels, read_run, write_run
from evaluate import ndcg_run
from rank import split_run


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("runs", nargs="+")
    ap.add_argument("--weights", default=None, help="pesos por run, ex: 1,1,0.5")
    ap.add_argument("--k", type=int, default=60, help="constante RRF")
    ap.add_argument("--out", default=None)
    args = ap.parse_args()

    runs = [read_run(p) for p in args.runs]
    w = ([float(x) for x in args.weights.split(",")] if args.weights
         else [1.0] * len(runs))
    assert len(w) == len(runs), "nº de pesos != nº de runs"

    qids = set().union(*[set(r) for r in runs])
    fused: dict[str, list[str]] = {}
    for qid in qids:
        scores: dict[str, float] = {}
        for run, wi in zip(runs, w):
            for rank, did in enumerate(run.get(qid, [])):
                scores[did] = scores.get(did, 0.0) + wi / (args.k + rank + 1)
        fused[qid] = [d for d, _ in sorted(scores.items(), key=lambda x: -x[1])][:100]

    qrels = load_qrels()
    train, test = split_run(fused)
    print(f"ensemble de {len(runs)} runs (pesos {w}, RRF k={args.k}): "
          f"nDCG@100 train = {ndcg_run(train, qrels):.4f}")
    for p, run in zip(args.runs, runs):
        tr, _ = split_run(run)
        print(f"  {p}: {ndcg_run(tr, qrels):.4f}")
    if args.out:
        write_run(test, args.out)
        print(f"submissão: {args.out}")


if __name__ == "__main__":
    main()
