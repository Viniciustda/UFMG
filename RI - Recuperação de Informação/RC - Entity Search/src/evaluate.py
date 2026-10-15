"""nDCG@100 nas train queries (validação offline antes de qualquer submissão).

Ganho exponencial (2^rel - 1) por padrão; --gain linear para comparar com o
Kaggle na calibração da 1ª submissão.

Uso: python src/evaluate.py runs/arquivo.csv [--k 100] [--gain exp|linear]
"""
from __future__ import annotations

import argparse
import math

from common import load_qrels, read_run


def dcg(rels: list[int], k: int, gain: str) -> float:
    g = (lambda r: (1 << r) - 1) if gain == "exp" else (lambda r: r)
    return sum(g(r) / math.log2(i + 2) for i, r in enumerate(rels[:k]))


def ndcg_run(
    run: dict[str, list[str]],
    qrels: dict[str, dict[str, int]],
    k: int = 100,
    gain: str = "exp",
    per_query: bool = False,
):
    vals = {}
    for qid, judged in qrels.items():
        ideal = sorted(judged.values(), reverse=True)
        idcg = dcg(ideal, k, gain)
        got = dcg([judged.get(d, 0) for d in run.get(qid, [])], k, gain)
        vals[qid] = got / idcg if idcg > 0 else 0.0
    mean = sum(vals.values()) / len(vals) if vals else 0.0
    return (mean, vals) if per_query else mean


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("run")
    ap.add_argument("--k", type=int, default=100)
    ap.add_argument("--gain", choices=("exp", "linear"), default="exp")
    args = ap.parse_args()
    qrels = load_qrels()
    run = read_run(args.run)
    covered = sum(1 for q in qrels if q in run)
    score = ndcg_run(run, qrels, args.k, args.gain)
    print(f"nDCG@{args.k} ({args.gain}) = {score:.4f}  [{covered}/{len(qrels)} queries no run]")


if __name__ == "__main__":
    main()
