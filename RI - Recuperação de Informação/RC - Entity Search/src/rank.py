"""Re-pontuação dos candidatos cacheados com parâmetros arbitrários (segundos).

Modelos: bm25 (campos concatenados) e bm25f (b e peso por campo).
Gera a submissão (test) e avalia nDCG@100 no train.

Uso:
  python src/rank.py --tag base --model bm25 --out runs/sub1_bm25.csv
  python src/rank.py --tag base --model bm25f --k1 0.9 --b 0.9,0.15,0.2 --w 7,1.1,1 --out runs/sub2.csv
"""
from __future__ import annotations

import argparse
import json
import pickle
from dataclasses import dataclass
from math import log
from pathlib import Path

import numpy as np

from common import CACHE, TEST_QUERIES, load_queries, load_qrels, write_run
from evaluate import ndcg_run


@dataclass
class Params:
    model: str = "bm25f"
    k1: float = 1.2
    b: tuple[float, float, float] = (0.75, 0.75, 0.75)
    w: tuple[float, float, float] = (1.0, 1.0, 1.0)


class CandRanker:
    def __init__(self, tag: str = "base"):
        with open(CACHE / f"cands_{tag}.pkl", "rb") as f:
            data = pickle.load(f)
        stats = json.loads((CACHE / "stats.json").read_text(encoding="utf-8"))
        self.N = stats["N"]
        self.avg = np.asarray(stats["avg_len"], dtype=np.float64)
        self.avg_all = float(self.avg.sum())
        df = stats["df"]
        try:
            with open(CACHE / "sample_stats.pkl", "rb") as f:
                samp = pickle.load(f)
            self.df_s, self.ratio = samp["df_s"], self.N / max(samp["sample_docs"], 1)
        except FileNotFoundError:
            self.df_s, self.ratio = {}, 1.0
        self.lens = np.load(CACHE / "lens.npy")
        self.ids = np.load(CACHE / "ids.npy")
        self.queries = data["queries"]          # [(qid, terms, weights)]
        self.cands = data["cands"]              # qid -> dict(rows, pre, tf)
        self.qids = [q for q, _, _ in self.queries]

        self._df = df
        self._q: dict[str, dict] = {}
        for qid, terms, weights in self.queries:
            c = self.cands[qid]
            dfs = np.array([self._get_df(t) for t in terms], dtype=np.float64)  # (T,4)
            idf = np.log((self.N - dfs + 0.5) / (dfs + 0.5) + 1.0)
            self._q[qid] = dict(
                terms=terms,
                qw=np.asarray(weights, dtype=np.float32),
                rows=c["rows"],
                tf=c["tf"],                      # uint8; converter no score
                pre=c["pre"],
                idf=idf.astype(np.float32),      # colunas: title, kw, text, any
                dlen=self.lens[c["rows"]].astype(np.float32),
            )

    def _get_df(self, t: str) -> list[float]:
        d = self._df.get(t)
        if d is not None and d[3] > 0:
            return d
        est = min(max(self.df_s.get(t, 0) * self.ratio, 1.0), self.N)
        return [est, est, est, est]

    def score(self, qid: str, p: Params) -> tuple[np.ndarray, np.ndarray]:
        q = self._q[qid]
        if len(q["rows"]) == 0:
            return q["rows"], np.zeros(0)
        tf, dlen = q["tf"].astype(np.float32), q["dlen"]
        if p.model == "bm25f":
            b = np.asarray(p.b, dtype=np.float32)
            norm = 1.0 - b + b * dlen / self.avg.astype(np.float32)           # (n,3)
            np.maximum(norm, 1e-6, out=norm)  # campo vazio com b=1 → norm 0
            tfn = np.einsum("ntf,f->nt", tf / norm[:, None, :], np.asarray(p.w, dtype=np.float32))
            s = (q["qw"] * q["idf"][:, 3] * (tfn / (p.k1 + tfn + 1e-12))).sum(axis=1)
        else:  # bm25 campos concatenados
            tfa = tf.sum(axis=2)                                              # (n,T)
            la = dlen.sum(axis=1)                                             # (n,)
            denom = tfa + p.k1 * (1.0 - p.b[0] + p.b[0] * la / self.avg_all)[:, None]
            s = (q["qw"] * q["idf"][:, 3] * (tfa * (p.k1 + 1.0) / (denom + 1e-12))).sum(axis=1)
        return q["rows"], s

    def per_field_bm25(self, qid: str, k1: float = 0.9, b: float = 0.4) -> np.ndarray:
        """BM25 por campo (n,3) — features de LTR."""
        q = self._q[qid]
        tf, dlen = q["tf"].astype(np.float32), q["dlen"]
        out = np.zeros((len(q["rows"]), 3))
        for f in range(3):
            norm = np.maximum(1.0 - b + b * dlen[:, f] / self.avg[f], 1e-6)
            tfn = tf[:, :, f] / norm[:, None]
            out[:, f] = (q["qw"] * q["idf"][:, f] * tfn / (k1 + tfn + 1e-12)).sum(axis=1)
        return out

    def run_all(self, p: Params, k: int = 100) -> dict[str, list[str]]:
        run = {}
        for qid in self.qids:
            rows, s = self.score(qid, p)
            order = np.argsort(-s, kind="stable")[:k]
            run[qid] = [self.ids[rows[i]].decode() for i in order]
        return run

    def pool_recall(self, ks=(100, 500, 1000, 2000)) -> dict[int, float]:
        qrels = load_qrels()
        out = {}
        for k in ks:
            vals = []
            for qid, judged in qrels.items():
                rel = {d for d, r in judged.items() if r > 0}
                if not rel:
                    continue
                rows = self._q[qid]["rows"]
                pre = self._q[qid]["pre"]
                top = rows[np.argsort(-pre, kind="stable")[:k]]
                got = {self.ids[r].decode() for r in top}
                vals.append(len(rel & got) / len(rel))
            out[k] = float(np.mean(vals))
        return out


def split_run(run: dict[str, list[str]]):
    test_qids = {q for q, _ in load_queries(TEST_QUERIES)}
    test = {q: v for q, v in run.items() if q in test_qids}
    train = {q: v for q, v in run.items() if q not in test_qids}
    return train, test


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--tag", default="base")
    ap.add_argument("--model", choices=("bm25", "bm25f"), default="bm25f")
    ap.add_argument("--k1", type=float, default=1.2)
    ap.add_argument("--b", default="0.75,0.75,0.75")
    ap.add_argument("--w", default="1,1,1")
    ap.add_argument("--params", default=None, help="json com {k1,b,w} (sobrepõe flags)")
    ap.add_argument("--out", default=None)
    ap.add_argument("--recall", action="store_true")
    args = ap.parse_args()

    if args.params:
        cfg = json.loads(Path(args.params).read_text(encoding="utf-8"))
        p = Params(model=args.model, k1=cfg["k1"], b=tuple(cfg["b"]), w=tuple(cfg["w"]))
    else:
        p = Params(model=args.model, k1=args.k1,
                   b=tuple(float(x) for x in args.b.split(",")),
                   w=tuple(float(x) for x in args.w.split(",")))

    cr = CandRanker(args.tag)
    if args.recall:
        print("recall do pool:", cr.pool_recall())
    run = cr.run_all(p)
    train, test = split_run(run)
    score = ndcg_run(train, load_qrels())
    print(f"nDCG@100 train = {score:.4f}  ({p})")
    if args.out:
        write_run(test, args.out)
        train_out = Path(args.out).with_name(Path(args.out).stem + "_train.csv")
        write_run(train, train_out)
        print(f"submissão: {args.out} | train run: {train_out}")


if __name__ == "__main__":
    main()
