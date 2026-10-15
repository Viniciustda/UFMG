"""LTR LambdaMART (LightGBM) sobre todos os sinais — estágio final.

Features por (query, doc) calculadas on-the-fly a partir do texto cru (uniforme
para docs lexicais e dense-only): BM25F tunado, BM25 por campo, coberturas
(simples e por IDF), match de título (exato/prefixo/Jaccard), comprimentos,
+ sinais dos estágios (lex, dense, rrf, CE zero-shot, CE fine-tuned OOF/ensemble).

CV 5-fold por query (mesmos folds do train_ce → feature ce_ft permanece OOF).
Só gerar submissão se a CV superar o melhor estágio anterior.

Uso:
  python src/ltr.py [--pool cache/pool_fused.csv.gz] [--ce cache/ce_scores.csv.gz]
      [--ceft cache/ce_ft_scores.csv.gz] [--depth 1000] [--out runs/sub6_ltr.csv]
"""
from __future__ import annotations

import argparse
import json
from math import log

import numpy as np
import pandas as pd

from common import CACHE, CORPUS, TEST_QUERIES, all_queries, load_queries, load_qrels, query_tokens, tokenize, write_run
from evaluate import ndcg_run
from rank import split_run

SEED = 13
BEST = json.loads(open(CACHE / "best_bm25f.json", encoding="utf-8").read()) \
    if (CACHE / "best_bm25f.json").exists() else dict(k1=0.9, b=[0.9, 0.15, 0.2], w=[16, 2, 1])


def fetch_fields(rows: set[int]) -> dict[int, tuple[str, str, str]]:
    import orjson

    out = {}
    with open(CORPUS, "rb") as f:
        for i, line in enumerate(f):
            if i in rows:
                d = orjson.loads(line)
                kw = d.get("keywords") or []
                if isinstance(kw, list):
                    kw = " ".join(str(x) for x in kw)
                out[i] = (str(d.get("title") or ""), str(kw), str(d.get("text") or ""))
    return out


def doc_features(qterms: list[str], qidf: np.ndarray, fields: tuple[str, str, str],
                 avg: list[float]) -> list[float]:
    toks = [tokenize(fields[0]), tokenize(fields[1]), tokenize(fields[2])]
    T = len(qterms)
    tf = np.zeros((T, 3))
    pos = {t: i for i, t in enumerate(qterms)}
    for f in range(3):
        for t in toks[f]:
            i = pos.get(t)
            if i is not None:
                tf[i, f] += 1
    k1, b, w = BEST["k1"], np.asarray(BEST["b"]), np.asarray(BEST["w"])
    lens = np.array([len(x) for x in toks], dtype=float)
    norm = np.maximum(1 - b + b * lens / np.asarray(avg), 1e-6)
    tfn = (tf / norm) @ w
    bm25f = float((qidf * tfn / (k1 + tfn + 1e-12)).sum())
    pf = []
    for f in range(3):
        x = tf[:, f] / norm[f]
        pf.append(float((qidf * x / (0.9 + x)).sum()))
    matched = tf.sum(axis=1) > 0
    cov = float(matched.mean())
    cov_idf = float(qidf[matched].sum() / (qidf.sum() + 1e-9))
    cov_title = float((tf[:, 0] > 0).mean())
    qset, tset = set(qterms), set(toks[0])
    jac = len(qset & tset) / (len(qset | tset) or 1)
    exact = 1.0 if toks[0] == qterms else 0.0
    prefix = 1.0 if toks[0][: len(qterms)] == qterms else 0.0
    return [bm25f, *pf, cov, cov_idf, cov_title, jac, exact, prefix,
            *np.log1p(lens).tolist()]


FEAT_NAMES = ["bm25f", "bm25_title", "bm25_kw", "bm25_text", "cov", "cov_idf",
              "cov_title", "jac_title", "exact_title", "prefix_title",
              "loglen_t", "loglen_k", "loglen_x",
              "lex", "lex_z", "dense", "dense_z", "rrf", "ce", "ce_z",
              "ceft", "ceft_z", "cev2", "cev2_z", "q_nterms", "q_len"]


def znorm_col(df, col):
    def z(g):
        s = g.std()
        return (g - g.mean()) / s if s > 1e-9 else g * 0.0
    return df.groupby("QueryId")[col].transform(z)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--pool", default=str(CACHE / "pool_fused.csv.gz"))
    ap.add_argument("--ce", default=str(CACHE / "ce_scores.csv.gz"))
    ap.add_argument("--ceft", default=str(CACHE / "ce_ft_scores.csv.gz"))
    ap.add_argument("--cev2", default=str(CACHE / "ce_v2_scores.csv.gz"))
    ap.add_argument("--depth", type=int, default=1000)
    ap.add_argument("--folds", type=int, default=5)
    ap.add_argument("--out", default=None)
    args = ap.parse_args()

    import lightgbm as lgb

    qrels = load_qrels()
    test_qids = {q for q, _ in load_queries(TEST_QUERIES)}
    queries = {qid: q for qid, q in all_queries()}
    stats = json.loads((CACHE / "stats.json").read_text(encoding="utf-8"))
    N, avg, dfm = stats["N"], stats["avg_len"], stats["df"]

    pool = pd.read_csv(args.pool, dtype={"QueryId": str, "EntityId": str})
    col = "rrf" if "rrf" in pool.columns else "score"
    pool = pool.sort_values(["QueryId", col], ascending=[True, False])
    pool = pool.groupby("QueryId").head(args.depth).reset_index(drop=True)

    for path, name in ((args.ce, "ce"), (args.ceft, "ceft"), (args.cev2, "cev2")):
        try:
            s = pd.read_csv(path, dtype={"QueryId": str, "EntityId": str})
            s = s[["QueryId", "EntityId", "ce_score"]].rename(columns={"ce_score": name})
            pool = pool.merge(s, on=["QueryId", "EntityId"], how="left")
        except FileNotFoundError:
            pool[name] = np.nan
            print(f"aviso: {path} ausente — feature {name} = NaN")

    print("features de texto...", flush=True)
    fields = fetch_fields(set(pool["row"].astype(int)))
    qcache: dict[str, tuple[list[str], np.ndarray]] = {}
    for qid in pool["QueryId"].unique():
        qts = query_tokens(queries[qid])
        seen = []
        for t in qts:
            if t not in seen:
                seen.append(t)
        d = np.array([dfm.get(t, [0, 0, 0, 1])[3] or 1 for t in seen], dtype=float)
        qcache[qid] = (seen, np.log((N - d + 0.5) / (d + 0.5) + 1.0))

    X_txt = np.zeros((len(pool), 13), dtype=np.float32)
    rows_np = pool["row"].astype(int).to_numpy()
    qids_np = pool["QueryId"].to_numpy()
    for i in range(len(pool)):
        qterms, qidf = qcache[qids_np[i]]
        X_txt[i] = doc_features(qterms, qidf, fields.get(rows_np[i], ("", "", "")), avg)
        if i % 100_000 == 0:
            print(f"  {i}/{len(pool)}", flush=True)

    pool["lex_z"] = znorm_col(pool, "lex")
    pool["dense_z"] = znorm_col(pool, "dense")
    pool["ce_z"] = znorm_col(pool, "ce")
    pool["ceft_z"] = znorm_col(pool, "ceft")
    pool["cev2_z"] = znorm_col(pool, "cev2")
    pool["q_nterms"] = pool["QueryId"].map(lambda q: len(qcache[q][0]))
    pool["q_len"] = pool["QueryId"].map(lambda q: len(queries[q]))

    extra = pool[["lex", "lex_z", "dense", "dense_z", "rrf", "ce", "ce_z",
                  "ceft", "ceft_z", "cev2", "cev2_z",
                  "q_nterms", "q_len"]].to_numpy(dtype=np.float32)
    X = np.hstack([X_txt, extra])
    y = np.array([qrels.get(q, {}).get(e, 0) for q, e in
                  zip(pool["QueryId"], pool["EntityId"])], dtype=np.int32)

    train_mask = ~pool["QueryId"].isin(test_qids).to_numpy()
    train_qids_sorted = sorted(qrels)
    fold_of = {qid: i % args.folds for i, qid in enumerate(train_qids_sorted)}
    fold_arr = pool["QueryId"].map(lambda q: fold_of.get(q, -1)).to_numpy()

    def make_ds(mask):
        order = np.argsort(qids_np[mask], kind="stable")
        idx = np.where(mask)[0][order]
        groups = pd.Series(qids_np[idx]).groupby(qids_np[idx], sort=False).size().tolist()
        return idx, lgb.Dataset(X[idx], label=y[idx], group=groups,
                                feature_name=FEAT_NAMES, free_raw_data=False)

    params = dict(objective="lambdarank", metric="ndcg", ndcg_eval_at=[100],
                  learning_rate=0.05, num_leaves=31, min_data_in_leaf=20,
                  feature_fraction=0.8, bagging_fraction=0.9, bagging_freq=1,
                  lambdarank_truncation_level=100, verbosity=-1, seed=SEED)

    oof_pred = np.full(len(pool), np.nan, dtype=np.float64)
    best_iters = []
    for f in range(args.folds):
        m_tr = train_mask & (fold_arr != f)
        m_ho = train_mask & (fold_arr == f)
        _, ds_tr = make_ds(m_tr)
        idx_ho, ds_ho = make_ds(m_ho)
        booster = lgb.train(params, ds_tr, num_boost_round=500, valid_sets=[ds_ho],
                            callbacks=[lgb.early_stopping(50, verbose=False)])
        best_iters.append(booster.best_iteration)
        oof_pred[idx_ho] = booster.predict(X[idx_ho], num_iteration=booster.best_iteration)
        print(f"fold {f}: iter {booster.best_iteration}", flush=True)

    dfo = pool[train_mask].copy()
    dfo["pred"] = oof_pred[train_mask]
    run = {q: g.sort_values("pred", ascending=False)["EntityId"].tolist()[:100]
           for q, g in dfo.groupby("QueryId")}
    cv = ndcg_run(run, qrels)
    print(f"LTR CV (OOF) nDCG@100 train = {cv:.4f}")
    imp = sorted(zip(FEAT_NAMES, booster.feature_importance("gain")),
                 key=lambda x: -x[1])[:12]
    print("importância (gain):", [(n, int(v)) for n, v in imp])

    if args.out:
        n_final = int(np.mean(best_iters))
        _, ds_all = make_ds(train_mask)
        final = lgb.train(params, ds_all, num_boost_round=n_final)
        m_test = ~train_mask
        pred = final.predict(X[m_test])
        dft = pool[m_test].copy()
        dft["pred"] = pred
        test_run = {q: g.sort_values("pred", ascending=False)["EntityId"].tolist()[:100]
                    for q, g in dft.groupby("QueryId")}
        write_run(test_run, args.out)
        print(f"submissão: {args.out} (iters={n_final})")


if __name__ == "__main__":
    main()
