"""RM3: expansão por pseudo-relevância sobre o ranking BM25F tunado.

P(t|R) ∝ Σ_D P(t|D)·P(D), com D = top-FB docs do BM25F e P(D) = score
normalizado. Filtros: termo em ≥2 fb docs, len ≥ 2, freq. de coleção < 1%.

Saídas:
  cache/rm3_pass.json  — [[termo, peso(λ=0.3)]] por qid (p/ a passada cands)
  cache/rm3_model.json — componentes crus {qid: {termo: [qtf, P(t|R)]}} (tuning de λ)

Uso: python src/rm3.py [--fb-docs 10] [--n-terms 25] [--params cache/best_bm25f.json]
"""
from __future__ import annotations

import argparse
import json
import pickle

import numpy as np
import orjson

from common import CACHE, CORPUS, FIELDS, tokenize
from rank import CandRanker, Params

PASS_LAMBDA = 0.3


def fetch_doc_tokens(rows: list[int], offsets: np.ndarray) -> dict[int, list[str]]:
    out = {}
    with open(CORPUS, "rb") as f:
        for r in sorted(set(rows)):
            f.seek(int(offsets[r]))
            doc = orjson.loads(f.readline())
            toks: list[str] = []
            for fld in FIELDS:
                v = doc.get(fld) or ""
                if isinstance(v, list):
                    v = " ".join(str(x) for x in v)
                toks.extend(tokenize(str(v)))
            out[r] = toks
    return out


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--fb-docs", type=int, default=10)
    ap.add_argument("--n-terms", type=int, default=25)
    ap.add_argument("--params", default=str(CACHE / "best_bm25f.json"))
    args = ap.parse_args()

    cfg = json.loads(open(args.params, encoding="utf-8").read())
    p = Params(model="bm25f", k1=cfg["k1"], b=tuple(cfg["b"]), w=tuple(cfg["w"]))
    cr = CandRanker("base")
    offsets = np.load(CACHE / "offsets.npy")
    with open(CACHE / "sample_stats.pkl", "rb") as f:
        samp = pickle.load(f)
    ctf_s, total_s = samp["ctf_s"], max(samp["sample_tokens"], 1)

    pass_out: dict[str, list] = {}
    model_out: dict[str, dict] = {}
    for qid, terms, weights in cr.queries:
        rows, s = cr.score(qid, p)
        if len(rows) == 0:
            pass_out[qid] = [[t, w] for t, w in zip(terms, weights)]
            model_out[qid] = {t: [w, 0.0] for t, w in zip(terms, weights)}
            continue
        order = np.argsort(-s, kind="stable")[: args.fb_docs]
        fb_rows = [int(rows[i]) for i in order]
        fb_scores = np.clip(s[order], 1e-6, None)
        pd = fb_scores / fb_scores.sum()
        docs = fetch_doc_tokens(fb_rows, offsets)

        prm: dict[str, float] = {}
        seen_in: dict[str, int] = {}
        for r, weight in zip(fb_rows, pd):
            toks = docs[r]
            if not toks:
                continue
            inv = 1.0 / len(toks)
            tf: dict[str, int] = {}
            for t in toks:
                tf[t] = tf.get(t, 0) + 1
            for t, c in tf.items():
                prm[t] = prm.get(t, 0.0) + float(weight) * c * inv
                seen_in[t] = seen_in.get(t, 0) + 1

        orig = set(terms)
        cand = [
            (t, v) for t, v in prm.items()
            if t not in orig and len(t) >= 2 and seen_in[t] >= 2
            and ctf_s.get(t, 0) / total_s < 0.01
        ]
        cand.sort(key=lambda x: -x[1])
        cand = cand[: args.n_terms]
        zc = sum(v for _, v in cand) or 1.0
        zq = sum(weights) or 1.0

        qtf = {t: w for t, w in zip(terms, weights)}
        model_out[qid] = {t: [qtf.get(t, 0.0), 0.0] for t in terms}
        for t, v in cand:
            model_out[qid][t] = [0.0, v / zc]
        lam = PASS_LAMBDA
        pass_out[qid] = (
            [[t, (1 - lam) * qtf[t] / zq] for t in terms]
            + [[t, lam * v / zc] for t, v in cand]
        )

    (CACHE / "rm3_pass.json").write_text(json.dumps(pass_out), encoding="utf-8")
    (CACHE / "rm3_model.json").write_text(json.dumps(model_out), encoding="utf-8")
    print(f"rm3 ok: {len(pass_out)} queries -> cache/rm3_pass.json (lam_pass={PASS_LAMBDA})")


if __name__ == "__main__":
    main()
