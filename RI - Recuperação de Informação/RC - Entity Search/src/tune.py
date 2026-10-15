"""Tuning de BM25F por descida de coordenadas no nDCG@100 do train.

Cada avaliação re-pontua o cache (~1s) — centenas de configs em minutos.
Saída: cache/best_bm25f.json

Uso: python src/tune.py [--tag base] [--rounds 4]
"""
from __future__ import annotations

import argparse
import json

from common import CACHE, load_qrels
from evaluate import ndcg_run
from rank import CandRanker, Params, split_run

GRID = {
    "k1": [0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.3, 1.6, 2.0],
    "b0": [0.0, 0.15, 0.3, 0.5, 0.7, 0.8, 0.9, 0.95, 1.0],
    "b1": [0.0, 0.1, 0.15, 0.2, 0.3, 0.5, 0.7, 0.9],
    "b2": [0.05, 0.1, 0.15, 0.2, 0.3, 0.5, 0.7, 0.9],
    "w0": [1, 2, 3, 5, 7, 9, 12, 16, 20, 26, 34, 44, 60],
    "w1": [0.3, 0.6, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0],
    "w2": [0.5, 1.0],  # texto ancora a escala
}
START = dict(k1=0.9, b0=0.9, b1=0.15, b2=0.2, w0=16.0, w1=2.0, w2=1.0)


def eval_cfg(cr: CandRanker, qrels, cfg) -> float:
    p = Params(model="bm25f", k1=cfg["k1"],
               b=(cfg["b0"], cfg["b1"], cfg["b2"]),
               w=(cfg["w0"], cfg["w1"], cfg["w2"]))
    train, _ = split_run(cr.run_all(p))
    return ndcg_run(train, qrels)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--tag", default="base")
    ap.add_argument("--rounds", type=int, default=4)
    args = ap.parse_args()

    cr = CandRanker(args.tag)
    qrels = load_qrels()
    cfg = dict(START)
    best = eval_cfg(cr, qrels, cfg)
    print(f"início: {best:.4f}  {cfg}")
    for r in range(args.rounds):
        improved = False
        for key, values in GRID.items():
            cur = cfg[key]
            for v in values:
                if v == cur:
                    continue
                trial = dict(cfg, **{key: v})
                s = eval_cfg(cr, qrels, trial)
                if s > best:
                    best, cfg, improved = s, trial, True
                    print(f"  [{r}] {key}={v} -> {best:.4f}", flush=True)
        if not improved:
            break
    out = dict(k1=cfg["k1"], b=[cfg["b0"], cfg["b1"], cfg["b2"]],
               w=[cfg["w0"], cfg["w1"], cfg["w2"]], ndcg_train=best)
    (CACHE / "best_bm25f.json").write_text(json.dumps(out, indent=1), encoding="utf-8")
    print(f"MELHOR: {best:.4f}  {out}")


if __name__ == "__main__":
    main()
