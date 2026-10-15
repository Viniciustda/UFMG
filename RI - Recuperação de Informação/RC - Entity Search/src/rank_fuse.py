"""BM25F + RM3 com fusão de pools (base ∪ rm3) — submissão 3.

O pool rm3 foi selecionado com a query expandida; o base, com a original.
A união é pontuada com a query interpolada (1-λ)·qtf + λ·P(t|R); candidatos
só do base entram com tf=0 nos termos de expansão (projeção).

Uso:
  python src/rank_fuse.py --out runs/sub3_rm3.csv            # tuna λ no train e gera
  python src/rank_fuse.py --lam 0.3 --export cache/pool.csv.gz --depth 2000
"""
from __future__ import annotations

import argparse
import csv
import gzip
import json

import numpy as np

from common import CACHE, load_qrels, write_run
from evaluate import ndcg_run
from rank import CandRanker, Params, split_run

LAM_GRID = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6]


class FusedRanker:
    def __init__(self, params_path=None):
        cfg = json.loads(open(params_path or CACHE / "best_bm25f.json", encoding="utf-8").read())
        self.p = Params(model="bm25f", k1=cfg["k1"], b=tuple(cfg["b"]), w=tuple(cfg["w"]))
        self.cb = CandRanker("base")
        self.cr = CandRanker("rm3")
        self.model = json.loads((CACHE / "rm3_model.json").read_text(encoding="utf-8"))
        self.qids = self.cr.qids
        self.ids = self.cr.ids
        self._m: dict[str, tuple] = {}

    def merged(self, qid: str):
        if qid in self._m:
            return self._m[qid]
        qr, qb = self.cr._q[qid], self.cb._q[qid]
        terms = qr["terms"]
        slot = {t: i for i, t in enumerate(terms)}
        rows_r = qr["rows"]
        have = set(rows_r.tolist())
        keep = [i for i, r in enumerate(qb["rows"].tolist()) if r not in have]
        T = len(terms)
        if keep:
            keep = np.asarray(keep, dtype=np.int64)
            proj = np.zeros((len(keep), T, 3), dtype=np.uint8)
            sub = qb["tf"][keep]
            for sb, t in enumerate(qb["terms"]):
                sr = slot.get(t)
                if sr is not None:
                    proj[:, sr, :] = sub[:, sb, :]
            rows = np.concatenate([rows_r, qb["rows"][keep]])
            tf = np.concatenate([qr["tf"], proj]) if len(rows_r) else proj
        else:
            rows, tf = rows_r, qr["tf"]
        dlen = self.cr.lens[rows].astype(np.float32)
        m = self.model[qid]
        qtfv = np.array([m.get(t, [0.0, 0.0])[0] for t in terms], dtype=np.float32)
        prmv = np.array([m.get(t, [0.0, 0.0])[1] for t in terms], dtype=np.float32)
        out = (rows, tf, dlen, qtfv, prmv, qr["idf"][:, 3])
        self._m[qid] = out
        return out

    def score(self, qid: str, lam: float):
        rows, tf, dlen, qtfv, prmv, idf = self.merged(qid)
        if len(rows) == 0:
            return rows, np.zeros(0, dtype=np.float32)
        zq = float(qtfv.sum()) or 1.0
        qw = (1.0 - lam) * qtfv / zq + lam * prmv
        b = np.asarray(self.p.b, dtype=np.float32)
        w = np.asarray(self.p.w, dtype=np.float32)
        norm = np.maximum(1.0 - b + b * dlen / self.cr.avg.astype(np.float32), 1e-6)
        tfn = np.einsum("ntf,f->nt", tf.astype(np.float32) / norm[:, None, :], w)
        s = (qw * idf * (tfn / (self.p.k1 + tfn + 1e-12))).sum(axis=1)
        return rows, s

    def run_all(self, lam: float, k: int = 100) -> dict[str, list[str]]:
        run = {}
        for qid in self.qids:
            rows, s = self.score(qid, lam)
            order = np.argsort(-s, kind="stable")[:k]
            run[qid] = [self.ids[rows[i]].decode() for i in order]
        return run

    def pool_recall(self, lam: float, ks=(100, 500, 1000, 2000)):
        qrels = load_qrels()
        out = {}
        ordered = {}
        for qid in qrels:
            rows, s = self.score(qid, lam)
            ordered[qid] = rows[np.argsort(-s, kind="stable")]
        for k in ks:
            vals = []
            for qid, judged in qrels.items():
                rel = {d for d, r in judged.items() if r > 0}
                if not rel:
                    continue
                got = {self.ids[r].decode() for r in ordered[qid][:k]}
                vals.append(len(rel & got) / len(rel))
            out[k] = round(float(np.mean(vals)), 4)
        return out


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--lam", type=float, default=None)
    ap.add_argument("--out", default=None)
    ap.add_argument("--export", default=None)
    ap.add_argument("--depth", type=int, default=2000)
    args = ap.parse_args()

    fr = FusedRanker()
    qrels = load_qrels()
    if args.lam is None:
        best = (-1.0, 0.0)
        for lam in LAM_GRID:
            train, _ = split_run(fr.run_all(lam))
            s = ndcg_run(train, qrels)
            print(f"λ={lam}: nDCG@100 train = {s:.4f}", flush=True)
            if s > best[0]:
                best = (s, lam)
        lam = best[1]
        print(f"MELHOR: λ={lam} ({best[0]:.4f})")
    else:
        lam = args.lam
        train, _ = split_run(fr.run_all(lam))
        print(f"λ={lam}: nDCG@100 train = {ndcg_run(train, qrels):.4f}")
    print("recall do pool fundido:", fr.pool_recall(lam))

    if args.out:
        _, test = split_run(fr.run_all(lam))
        write_run(test, args.out)
        train, _ = split_run(fr.run_all(lam))
        write_run(train, str(args.out).replace(".csv", "_train.csv"))
        print(f"submissão: {args.out}")
    if args.export:
        n = 0
        with gzip.open(args.export, "wt", newline="", encoding="utf-8") as f:
            w = csv.writer(f)
            w.writerow(["QueryId", "EntityId", "row", "score"])
            for qid in fr.qids:
                rows, s = fr.score(qid, lam)
                order = np.argsort(-s, kind="stable")[: args.depth]
                for i in order:
                    w.writerow([qid, fr.ids[rows[i]].decode(), int(rows[i]), f"{float(s[i]):.5f}"])
                    n += 1
        print(f"{n} pares exportados -> {args.export} (λ={lam})")


if __name__ == "__main__":
    main()
