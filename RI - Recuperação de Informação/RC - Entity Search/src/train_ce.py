"""Fine-tuning do cross-encoder nos qrels do train — CV 5-fold por query.

- Pares RankNet: positivo (rel 1/2) vs. hard negative (pool, não julgado);
  pares graduados rel2>rel1. Loss -log σ(s_pos - s_neg).
- Sem vazamento: scores do train vêm SEMPRE do modelo do fold que não viu a query
  (out-of-fold); scores do test = média dos 5 modelos (ensemble grátis).
- Saída: cache/ce_ft_scores.csv.gz (mesmo formato do rerank zero-shot →
  blend/submissão via: python src/rerank_ce.py --scores ... --out runs/sub5.csv)

Uso:
  python src/train_ce.py [--pool cache/pool_fused.csv.gz] [--depth 1000]
      [--model BAAI/bge-reranker-base] [--epochs 2] [--batch 8] [--accum 2]
"""
from __future__ import annotations

import argparse
import pickle
import random
import time
from pathlib import Path

import numpy as np
import pandas as pd

from common import CACHE, TEST_QUERIES, all_queries, load_queries, load_qrels
from evaluate import ndcg_run
from rerank_ce import fetch_texts

SEED = 13


def judged_rows(qrels) -> dict[str, int]:
    """EntityId -> row no corpus, via ids.npy (sem carregar dict de 4,6M)."""
    ids = np.load(CACHE / "ids.npy")
    need = {d.encode() for docs in qrels.values() for d in docs}
    mask = np.isin(ids, np.array(sorted(need), dtype=ids.dtype))
    rows = np.nonzero(mask)[0]
    return {ids[r].decode(): int(r) for r in rows}


def build_pairs(qids, pool_by_q, qrels, neg_top, ppp, rng, neg_skip=0):
    """[(qid, row_pos, row_neg, peso)] — negatives do pool em [neg_skip, neg_top).

    neg_skip>0 evita falsos negativos: docs não-julgados no topo do pool têm
    boa chance de serem relevantes (qrels incompletos em queries de lista)."""
    pairs = []
    for qid in qids:
        judged = qrels.get(qid, {})
        if not judged:
            continue
        cand = pool_by_q.get(qid, [])
        negs = [r for r, did in cand[neg_skip:neg_top] if did not in judged]
        if not negs:
            continue
        pos = [(r, rel) for did, (r, rel) in judged.items() if r is not None]
        by_rel = {1: [], 2: []}
        for r, rel in pos:
            by_rel.setdefault(rel, []).append(r)
            for _ in range(ppp):
                pairs.append((qid, r, negs[rng.randrange(len(negs))], float(rel)))
        g2, g1 = by_rel.get(2, []), by_rel.get(1, [])
        rng.shuffle(g2)
        for i, rp in enumerate(g2[:20]):
            if g1:
                pairs.append((qid, rp, g1[rng.randrange(len(g1))], 0.5))
    rng.shuffle(pairs)
    return pairs


def train_fold(model_name, pairs, texts, queries, args, dev="cuda"):
    import torch
    from torch.optim import AdamW
    from transformers import (AutoModelForSequenceClassification, AutoTokenizer,
                              get_linear_schedule_with_warmup)

    torch.manual_seed(SEED)
    tok = AutoTokenizer.from_pretrained(model_name)
    model = AutoModelForSequenceClassification.from_pretrained(model_name)
    if args.grad_ckpt:
        model.gradient_checkpointing_enable()
    if args.freeze_emb:
        getattr(model, model.base_model_prefix).embeddings.requires_grad_(False)
    model.to(dev).train()
    opt = AdamW(model.parameters(), lr=args.lr, weight_decay=0.01)
    steps = (len(pairs) * args.epochs) // (args.batch * args.accum) + 1
    sched = get_linear_schedule_with_warmup(opt, int(0.1 * steps), steps)
    scaler = torch.amp.GradScaler("cuda")

    t0, seen = time.time(), 0
    for ep in range(args.epochs):
        for bi in range(0, len(pairs), args.batch):
            chunk = pairs[bi: bi + args.batch]
            qs = [queries[q] for q, _, _, _ in chunk] * 2
            ds = ([texts.get(rp, "") for _, rp, _, _ in chunk]
                  + [texts.get(rn, "") for _, _, rn, _ in chunk])
            w = torch.tensor([wt for _, _, _, wt in chunk], device=dev)
            enc = tok(qs, ds, padding=True, truncation=True,
                      max_length=args.maxlen, return_tensors="pt").to(dev)
            with torch.autocast("cuda", dtype=torch.float16):
                logits = model(**enc).logits.squeeze(-1)
                sp, sn = logits[: len(chunk)], logits[len(chunk):]
                loss = -(torch.nn.functional.logsigmoid(sp - sn) * w).mean() / args.accum
            scaler.scale(loss).backward()
            if (bi // args.batch + 1) % args.accum == 0:
                scaler.unscale_(opt)
                torch.nn.utils.clip_grad_norm_(model.parameters(), 1.0)
                scaler.step(opt)
                scaler.update()
                opt.zero_grad(set_to_none=True)
                sched.step()
            seen += len(chunk)
            if bi % (args.batch * 200) == 0:
                rate = seen / max(time.time() - t0, 1e-9)
                print(f"    ep{ep} {seen}/{len(pairs)*args.epochs} pares "
                      f"({rate:.0f}/s, loss {float(loss)*args.accum:.4f})", flush=True)
    model.eval()
    return tok, model


def score_pairs(tok, model, items, texts, queries, args, dev="cuda"):
    """items: [(qid, row)] -> scores np.float32"""
    import torch

    out = np.zeros(len(items), dtype=np.float32)
    bs = args.batch * 8
    with torch.no_grad():
        for bi in range(0, len(items), bs):
            chunk = items[bi: bi + bs]
            enc = tok([queries[q] for q, _ in chunk],
                      [texts.get(r, "") for _, r in chunk],
                      padding=True, truncation=True, max_length=args.maxlen,
                      return_tensors="pt").to(dev)
            with torch.autocast("cuda", dtype=torch.float16):
                logits = model(**enc).logits.squeeze(-1)
            out[bi: bi + bs] = logits.float().cpu().numpy()
    return out


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--pool", default=str(CACHE / "pool_fused.csv.gz"))
    ap.add_argument("--col", default=None, help="coluna de ordenação do pool (default: rrf ou score)")
    ap.add_argument("--depth", type=int, default=1000)
    ap.add_argument("--model", default="BAAI/bge-reranker-base")
    ap.add_argument("--folds", type=int, default=5)
    ap.add_argument("--epochs", type=int, default=2)
    ap.add_argument("--batch", type=int, default=8)
    ap.add_argument("--accum", type=int, default=2)
    ap.add_argument("--maxlen", type=int, default=256)
    ap.add_argument("--lr", type=float, default=2e-5)
    ap.add_argument("--neg-top", type=int, default=200)
    ap.add_argument("--neg-skip", type=int, default=0)
    ap.add_argument("--pairs-per-pos", type=int, default=8)
    ap.add_argument("--grad-ckpt", action="store_true")
    ap.add_argument("--freeze-emb", action="store_true")
    ap.add_argument("--save-models", action="store_true")
    ap.add_argument("--out", default=str(CACHE / "ce_ft_scores.csv.gz"))
    args = ap.parse_args()
    rng = random.Random(SEED)

    queries = {qid: q for qid, q in all_queries()}
    qrels = load_qrels()
    test_qids = {q for q, _ in load_queries(TEST_QUERIES)}

    pool = pd.read_csv(args.pool, dtype={"QueryId": str, "EntityId": str})
    col = args.col or ("rrf" if "rrf" in pool.columns else "score")
    lexcol = "lex" if "lex" in pool.columns else col
    pool = pool.sort_values(["QueryId", col], ascending=[True, False])
    pool = pool.groupby("QueryId").head(args.depth).reset_index(drop=True)
    pool_by_q: dict[str, list[tuple[int, str]]] = {
        qid: list(zip(g["row"].astype(int), g["EntityId"]))
        for qid, g in pool.groupby("QueryId")
    }

    jrows = judged_rows(qrels)
    qrels_rows = {qid: {did: (jrows.get(did), rel) for did, rel in docs.items()}
                  for qid, docs in qrels.items()}
    need = set(pool["row"].astype(int)) | {r for r in jrows.values()}
    texts = fetch_texts(need)

    train_qids = sorted(qrels)
    folds = {qid: i % args.folds for i, qid in enumerate(train_qids)}

    oof: dict[tuple[str, int], float] = {}
    test_items = [(qid, int(r)) for qid in sorted(test_qids)
                  for r, _ in pool_by_q.get(qid, [])]
    test_acc = np.zeros(len(test_items), dtype=np.float64)

    stem = Path(args.out).name.split(".")[0]
    legacy = stem == "ce_ft_scores"  # checkpoints da 1ª rodada usavam outro nome
    for f in range(args.folds):
        ckpt = Path(args.out).parent / (f"ce_ft_fold{f}.pkl" if legacy
                                        else f"{stem}_fold{f}.pkl")
        if ckpt.exists():
            with open(ckpt, "rb") as fh:
                saved = pickle.load(fh)
            if saved.get("n_test") == len(test_items):
                oof.update(saved["oof"])
                test_acc += saved["test"]
                print(f"fold {f}: checkpoint carregado, pulando", flush=True)
                continue
        tr = [q for q in train_qids if folds[q] != f]
        ho = [q for q in train_qids if folds[q] == f]
        pairs = build_pairs(tr, pool_by_q, qrels_rows, args.neg_top,
                            args.pairs_per_pos, rng, args.neg_skip)
        print(f"fold {f}: {len(tr)} queries treino, {len(ho)} held-out, "
              f"{len(pairs)} pares", flush=True)
        tok, model = train_fold(args.model, pairs, texts, queries, args)
        if args.save_models:
            mdir = Path(args.out).parent / f"{stem}_model_f{f}"
            model.save_pretrained(mdir)
            tok.save_pretrained(mdir)

        ho_items = [(qid, int(r)) for qid in ho for r, _ in pool_by_q.get(qid, [])]
        s = score_pairs(tok, model, ho_items, texts, queries, args)
        fold_oof = {(qid, r): float(v) for (qid, r), v in zip(ho_items, s)}
        oof.update(fold_oof)
        print(f"  fold {f}: {len(ho_items)} OOF pontuados", flush=True)
        s_test = score_pairs(tok, model, test_items, texts, queries, args)
        test_acc += s_test
        with open(ckpt, "wb") as fh:
            pickle.dump(dict(oof=fold_oof, test=s_test, n_test=len(test_items)), fh)
        print(f"  fold {f}: test ensemble parcial ok (checkpoint salvo)", flush=True)
        import torch
        del model
        torch.cuda.empty_cache()

    test_scores = {it: v / args.folds for it, v in zip(test_items, test_acc)}
    rows_out = []
    for qid, g in pool.groupby("QueryId"):
        for _, r in g.iterrows():
            key = (qid, int(r["row"]))
            ce = oof.get(key) if qid in qrels else test_scores.get(key)
            if ce is None:
                continue
            rows_out.append((qid, r["EntityId"], int(r["row"]),
                             float(r[lexcol]) if lexcol in g.columns else 0.0, ce))
    out = pd.DataFrame(rows_out, columns=["QueryId", "EntityId", "row", "lex", "ce_score"])
    out.to_csv(args.out, index=False, compression="gzip")
    print(f"scores salvos -> {args.out}")

    # avaliação rápida: CE puro no train (OOF)
    run = {}
    for qid, g in out[~out.QueryId.isin(test_qids)].groupby("QueryId"):
        run[qid] = g.sort_values("ce_score", ascending=False)["EntityId"].tolist()[:100]
    print(f"CE fine-tuned puro (OOF) nDCG@100 train = {ndcg_run(run, qrels):.4f}")


if __name__ == "__main__":
    main()
