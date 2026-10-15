"""Passadas streaming sobre o corpus → índice invertido parcial + cache de tf.

O conjunto de queries é fixo (467), então não construímos índice invertido
completo: duas passadas document-at-a-time bastam e o cache resultante permite
re-pontuar com QUAISQUER parâmetros (BM25/BM25F/RM3) em segundos.

  stats: N, df exato por termo de query × campo, comprimentos por campo,
         ids, offsets de linha; amostra 1/20 (df/ctf) p/ pesos RM3.
  cands: top-M=2000 candidatos/query por pré-score BM25F + docs julgados do
         train forçados no pool; salva tf por (candidato, termo, campo).

Cada fatia salva um part-file (retomável); o merge roda no processo pai.

Uso:
  python src/score_corpus.py stats [--workers 10]
  python src/score_corpus.py cands [--tag base] [--expansion cache/rm3_terms.json]
"""
from __future__ import annotations

import argparse
import heapq
import json
import pickle
import time
from math import log
from pathlib import Path

import numpy as np
import orjson

from common import CACHE, CORPUS, FIELDS, all_queries, load_qrels, query_tokens

M_TOP = 2000              # candidatos por query (pós-merge)
SAMPLE_EVERY = 20         # amostragem p/ stats de RM3
PRE_K1, PRE_B = 1.2, 0.5  # pré-score BM25F (apenas SELEÇÃO; afinação no rank.py)
PRE_W = (3.0, 2.0, 1.0)   # title, keywords, text

SLICES_META = CACHE / "slices.json"


# ---------------------------------------------------------------- helpers

def field_tokens(doc: dict) -> list[list[str]]:
    from common import tokenize

    out = []
    for f in FIELDS:
        v = doc.get(f) or ""
        if isinstance(v, list):
            v = " ".join(str(x) for x in v)
        out.append(tokenize(str(v)))
    return out


def build_queries(expansion_path: str | None):
    """[(qid, [termos], [pesos])]: base = qtf; expansão = pesos do json RM3."""
    exp = {}
    if expansion_path:
        exp = json.loads(Path(expansion_path).read_text(encoding="utf-8"))
    out = []
    for qid, text in all_queries():
        if qid in exp:
            terms = [t for t, _ in exp[qid]]
            weights = [float(w) for _, w in exp[qid]]
        else:
            tw: dict[str, float] = {}
            for t in query_tokens(text):
                tw[t] = tw.get(t, 0.0) + 1.0
            terms, weights = list(tw), list(tw.values())
        out.append((qid, terms, weights))
    return out


def slice_bounds(nslices: int) -> list[tuple[int, int]]:
    size = CORPUS.stat().st_size
    step = size // nslices + 1
    return [(i * step, min((i + 1) * step, size)) for i in range(nslices)]


def iter_slice(start: int, end: int):
    """Gera (offset_absoluto, linha_bytes) das linhas que COMEÇAM em [start, end)."""
    with open(CORPUS, "rb") as f:
        pos = start
        f.seek(start)
        if start > 0:
            pos += len(f.readline())  # alinha no início da próxima linha
        while pos < end:
            line = f.readline()
            if not line:
                break
            yield pos, line
            pos += len(line)


# ---------------------------------------------------------------- fase 1: stats

_P1: dict = {}


def _init_p1(vocab_terms: list[str]):
    _P1["vocab"] = set(vocab_terms)


def _run_p1(job):
    si, start, end = job
    part = CACHE / f"p1_part{si}.pkl"
    if part.exists():
        return si
    vocab = _P1["vocab"]
    ids: list[bytes] = []
    offs: list[int] = []
    lens: list[tuple[int, int, int]] = []
    df: dict[str, np.ndarray] = {}
    df_s: dict[str, int] = {}
    ctf_s: dict[str, int] = {}
    sample_docs = sample_tokens = 0
    n = 0
    for pos, line in iter_slice(start, end):
        doc = orjson.loads(line)
        toks = field_tokens(doc)
        ids.append(str(doc["id"]).encode())
        offs.append(pos)
        lens.append(tuple(min(len(t), 65535) for t in toks))
        seen_any: set[str] = set()
        for fi, ft in enumerate(toks):
            for t in set(ft):
                if t in vocab:
                    a = df.get(t)
                    if a is None:
                        a = df[t] = np.zeros(4, dtype=np.int64)
                    a[fi] += 1
                    seen_any.add(t)
        for t in seen_any:
            df[t][3] += 1
        if n % SAMPLE_EVERY == 0:
            sample_docs += 1
            doc_terms: set[str] = set()
            for ft in toks:
                sample_tokens += len(ft)
                for t in ft:
                    ctf_s[t] = ctf_s.get(t, 0) + 1
                    doc_terms.add(t)
            for t in doc_terms:
                df_s[t] = df_s.get(t, 0) + 1
        n += 1
    with open(part, "wb") as f:
        pickle.dump(
            dict(nrows=n, ids=np.array(ids, dtype="S8"), offsets=np.array(offs, dtype=np.int64),
                 lens=np.array(lens, dtype=np.uint16), df=df, df_s=df_s, ctf_s=ctf_s,
                 sample_docs=sample_docs, sample_tokens=sample_tokens),
            f, protocol=4)
    return si


def merge_p1(nslices: int, bounds) -> None:
    ids_l, offs_l, lens_l = [], [], []
    df: dict[str, np.ndarray] = {}
    df_s: dict[str, int] = {}
    ctf_s: dict[str, int] = {}
    rows_per_slice = []
    sample_docs = sample_tokens = 0
    for si in range(nslices):
        with open(CACHE / f"p1_part{si}.pkl", "rb") as f:
            p = pickle.load(f)
        rows_per_slice.append(p["nrows"])
        ids_l.append(p["ids"]); offs_l.append(p["offsets"]); lens_l.append(p["lens"])
        for t, a in p["df"].items():
            if t in df:
                df[t] += a
            else:
                df[t] = a
        for t, c in p["df_s"].items():
            df_s[t] = df_s.get(t, 0) + c
        for t, c in p["ctf_s"].items():
            ctf_s[t] = ctf_s.get(t, 0) + c
        sample_docs += p["sample_docs"]; sample_tokens += p["sample_tokens"]
    ids = np.concatenate(ids_l); offsets = np.concatenate(offs_l); lens = np.concatenate(lens_l)
    N = len(ids)
    np.save(CACHE / "ids.npy", ids)
    np.save(CACHE / "offsets.npy", offsets)
    np.save(CACHE / "lens.npy", lens)
    stats = dict(
        N=N,
        avg_len=[float(lens[:, f].mean()) for f in range(3)],
        df={t: a.tolist() for t, a in df.items()},
        sample_docs=sample_docs, sample_tokens=sample_tokens,
        sample_every=SAMPLE_EVERY,
    )
    (CACHE / "stats.json").write_text(json.dumps(stats), encoding="utf-8")
    with open(CACHE / "sample_stats.pkl", "wb") as f:
        pickle.dump(dict(df_s={t: c for t, c in df_s.items() if c >= 2},
                         ctf_s={t: c for t, c in ctf_s.items() if c >= 2},
                         sample_docs=sample_docs, sample_tokens=sample_tokens), f, protocol=4)
    SLICES_META.write_text(json.dumps(dict(nslices=nslices, bounds=bounds,
                                           rows_per_slice=rows_per_slice)), encoding="utf-8")
    print(f"fase 1 ok: N={N}, vocab df={len(df)}, avg_len={stats['avg_len']}")


# ---------------------------------------------------------------- fase 2: candidatos

_P2: dict = {}


def _pack(hits, tfd) -> bytes:
    """(slot, tf_title, tf_kw, tf_text) em 4 bytes por hit (slot e tf cap 255)."""
    b = bytearray()
    for slot, tid in hits:
        tf = tfd[tid]
        b += bytes((min(slot, 255), min(tf[0], 255), min(tf[1], 255), min(tf[2], 255)))
    return bytes(b)


def _unpack_into(tf_row: np.ndarray, packed: bytes) -> None:
    for i in range(0, len(packed), 4):
        sl = packed[i]
        tf_row[sl, 0] = packed[i + 1]
        tf_row[sl, 1] = packed[i + 2]
        tf_row[sl, 2] = packed[i + 3]


def _init_p2(expansion_path: str | None, tag: str):
    queries = build_queries(expansion_path)
    stats = json.loads((CACHE / "stats.json").read_text(encoding="utf-8"))
    N = stats["N"]
    df_exact = stats["df"]
    ratio = N / max(stats["sample_docs"], 1)
    df_s = {}
    if expansion_path:
        with open(CACHE / "sample_stats.pkl", "rb") as f:
            df_s = pickle.load(f)["df_s"]

    vocab: dict[str, int] = {}
    qpost: list[list[tuple[int, int]]] = []   # tid -> [(q_idx, slot)]
    idf: list[float] = []
    qweights: list[np.ndarray] = []
    qnterms: list[int] = []
    for qi, (_qid, terms, weights) in enumerate(queries):
        for slot, t in enumerate(terms):
            tid = vocab.get(t)
            if tid is None:
                tid = vocab[t] = len(vocab)
                qpost.append([])
                d = df_exact.get(t, [0, 0, 0, 0])[3]
                if d == 0:
                    d = min(int(df_s.get(t, 0) * ratio) or 1, N)
                idf.append(log((N - d + 0.5) / (d + 0.5) + 1.0))
            qpost[tid].append((qi, slot))
        qweights.append(np.asarray(weights, dtype=np.float32))
        qnterms.append(len(terms))
    _P2.update(
        vocab=vocab, qpost=qpost, idf=idf, qweights=qweights, qnterms=qnterms,
        queries=queries, avg=stats["avg_len"], tag=tag,
        judged={}, m_top=M_TOP,
    )
    qid_to_qi = {qid: qi for qi, (qid, _, _) in enumerate(queries)}
    judged: dict[str, list[int]] = {}
    for qid, docs in load_qrels().items():
        qi = qid_to_qi[qid]
        for did in docs:
            judged.setdefault(did, []).append(qi)
    _P2["judged"] = judged


def _run_p2(job):
    si, start, end, row_start = job
    tag = _P2["tag"]
    part = CACHE / f"p2_{tag}_part{si}.pkl"
    if part.exists():
        return si
    vocab = _P2["vocab"]; qpost = _P2["qpost"]; idf = _P2["idf"]
    qweights = _P2["qweights"]; qnterms = _P2["qnterms"]
    avg = _P2["avg"]; judged = _P2["judged"]; m = _P2["m_top"]
    nq = len(qweights)
    heaps: list[list] = [[] for _ in range(nq)]
    forced: dict[int, list] = {}
    counter = 0
    row = row_start
    inv_avg = [1.0 / a for a in avg]
    for _pos, line in iter_slice(start, end):
        doc = orjson.loads(line)
        toks = field_tokens(doc)
        tfd: dict[int, list[int]] = {}
        for fi, ft in enumerate(toks):
            for t in ft:
                tid = vocab.get(t)
                if tid is not None:
                    a = tfd.get(tid)
                    if a is None:
                        a = tfd[tid] = [0, 0, 0]
                    a[fi] += 1
        if tfd or str(doc["id"]) in judged:
            norm = [1.0 - PRE_B + PRE_B * len(toks[f]) * inv_avg[f] for f in range(3)]
            qhits: dict[int, list[tuple[int, int]]] = {}
            for tid in tfd:
                for qi, slot in qpost[tid]:
                    qhits.setdefault(qi, []).append((slot, tid))
            for qi, hits in qhits.items():
                w = qweights[qi]
                s = 0.0
                for slot, tid in hits:
                    tf = tfd[tid]
                    wtf = (PRE_W[0] * tf[0] / norm[0] + PRE_W[1] * tf[1] / norm[1]
                           + PRE_W[2] * tf[2] / norm[2])
                    s += float(w[slot]) * idf[tid] * wtf / (PRE_K1 + wtf)
                h = heaps[qi]
                if len(h) < m:
                    counter += 1
                    heapq.heappush(h, (s, counter, row, _pack(hits, tfd)))
                elif s > h[0][0]:
                    counter += 1
                    heapq.heapreplace(h, (s, counter, row, _pack(hits, tfd)))
            for qi in judged.get(str(doc["id"]), ()):
                forced.setdefault(qi, []).append((row, _pack(qhits.get(qi, []), tfd)))
        row += 1

    # compacta: arrays por query
    out_q = {}
    for qi in range(nq):
        items = sorted(heaps[qi], key=lambda x: -x[0])
        T = qnterms[qi]
        rows = np.array([it[2] for it in items], dtype=np.int64)
        pre = np.array([it[0] for it in items], dtype=np.float32)
        tf = np.zeros((len(items), T, 3), dtype=np.uint8)
        for i, it in enumerate(items):
            _unpack_into(tf[i], it[3])
        out_q[qi] = (rows, pre, tf)
    out_f = {}
    for qi, lst in forced.items():
        T = qnterms[qi]
        rows = np.array([r for r, _ in lst], dtype=np.int64)
        tf = np.zeros((len(lst), T, 3), dtype=np.uint8)
        for i, (_r, sp) in enumerate(lst):
            _unpack_into(tf[i], sp)
        out_f[qi] = (rows, tf)
    with open(part, "wb") as f:
        pickle.dump(dict(per_q=out_q, forced=out_f), f, protocol=4)
    return si


def merge_p2(nslices: int, tag: str, expansion_path: str | None) -> None:
    queries = build_queries(expansion_path)
    nq = len(queries)
    best: list[list] = [[] for _ in range(nq)]   # (pre, row, tf_row)
    forced: list[dict] = [dict() for _ in range(nq)]  # row -> tf_row
    for si in range(nslices):
        with open(CACHE / f"p2_{tag}_part{si}.pkl", "rb") as f:
            p = pickle.load(f)
        for qi, (rows, pre, tf) in p["per_q"].items():
            b = best[qi]
            for i in range(len(rows)):
                b.append((float(pre[i]), int(rows[i]), tf[i]))
            if len(b) > 3 * M_TOP:
                b.sort(key=lambda x: -x[0])
                del b[M_TOP:]
        for qi, (rows, tf) in p["forced"].items():
            for i in range(len(rows)):
                forced[qi][int(rows[i])] = tf[i]
    out = {}
    for qi, (qid, terms, weights) in enumerate(queries):
        b = best[qi]
        b.sort(key=lambda x: -x[0])
        b = b[:M_TOP]
        have = {r for _, r, _ in b}
        extra = [(0.0, r, tfr) for r, tfr in forced[qi].items() if r not in have]
        allc = b + extra
        T = len(terms)
        rows = np.array([r for _, r, _ in allc], dtype=np.int64)
        pre = np.array([s for s, _, _ in allc], dtype=np.float32)
        tf = (np.stack([t for _, _, t in allc]) if allc
              else np.zeros((0, T, 3), dtype=np.uint8))
        out[qid] = dict(rows=rows, pre=pre, tf=tf)
    with open(CACHE / f"cands_{tag}.pkl", "wb") as f:
        pickle.dump(dict(
            queries=[(qid, terms, weights) for qid, terms, weights in queries],
            cands=out), f, protocol=4)
    sizes = [len(v["rows"]) for v in out.values()]
    print(f"fase 2 [{tag}] ok: {nq} queries, pool médio {np.mean(sizes):.0f} "
          f"(min {min(sizes)}, max {max(sizes)}) -> cache/cands_{tag}.pkl")


# ---------------------------------------------------------------- main

def main() -> None:
    import multiprocessing as mp

    ap = argparse.ArgumentParser()
    ap.add_argument("phase", choices=("stats", "cands"))
    ap.add_argument("--workers", type=int, default=10)
    ap.add_argument("--tag", default="base")
    ap.add_argument("--expansion", default=None)
    ap.add_argument("--limit-mb", type=int, default=0, help="processa só os primeiros X MB (smoke test)")
    args = ap.parse_args()

    t0 = time.time()
    if args.phase == "stats":
        nslices = args.workers
        bounds = slice_bounds(nslices)
        if args.limit_mb:
            cut = args.limit_mb * 1024 * 1024
            bounds = [(s, min(e, cut)) for s, e in bounds if s < cut]
            nslices = len(bounds)
        vocab = sorted({t for _, terms, _ in build_queries(None) for t in terms})
        print(f"fase 1: {nslices} fatias, vocab={len(vocab)} termos")
        jobs = [(si, s, e) for si, (s, e) in enumerate(bounds)]
        with mp.Pool(args.workers, initializer=_init_p1, initargs=(vocab,)) as pool:
            for si in pool.imap_unordered(_run_p1, jobs):
                print(f"  fatia {si} ok ({time.time()-t0:.0f}s)", flush=True)
        merge_p1(nslices, bounds)
    else:
        meta = json.loads(SLICES_META.read_text(encoding="utf-8"))
        bounds = meta["bounds"]
        row_starts = np.concatenate([[0], np.cumsum(meta["rows_per_slice"])[:-1]]).tolist()
        jobs = [(si, s, e, int(row_starts[si])) for si, (s, e) in enumerate(bounds)]
        print(f"fase 2 [{args.tag}]: {len(jobs)} fatias")
        with mp.Pool(args.workers, initializer=_init_p2,
                     initargs=(args.expansion, args.tag)) as pool:
            for si in pool.imap_unordered(_run_p2, jobs):
                print(f"  fatia {si} ok ({time.time()-t0:.0f}s)", flush=True)
        merge_p2(len(jobs), args.tag, args.expansion)
        for si in range(len(jobs)):
            (CACHE / f"p2_{args.tag}_part{si}.pkl").unlink(missing_ok=True)
    print(f"total: {time.time()-t0:.0f}s")


if __name__ == "__main__":
    main()
