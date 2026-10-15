"""Valida uma submissão antes do upload (formato, cobertura, duplicatas).

Uso: python src/check_submission.py runs/subX.csv
"""
from __future__ import annotations

import argparse
import csv
import sys
from pathlib import Path

from common import CACHE, TEST_QUERIES, load_queries


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("sub")
    args = ap.parse_args()

    test_qids = {q for q, _ in load_queries(TEST_QUERIES)}
    errors: list[str] = []
    per_q: dict[str, list[str]] = {}

    with open(args.sub, newline="", encoding="utf-8") as f:
        header = f.readline().strip()
        if header != "QueryId,EntityId":
            errors.append(f"header inválido: {header!r}")
        for ln, row in enumerate(csv.reader(f), start=2):
            if len(row) != 2:
                errors.append(f"linha {ln}: {row!r}")
                continue
            qid, did = row
            per_q.setdefault(qid, []).append(did)

    extra = set(per_q) - test_qids
    missing = test_qids - set(per_q)
    if extra:
        errors.append(f"{len(extra)} qids fora do test set: {sorted(extra)[:5]}...")
    if missing:
        errors.append(f"{len(missing)} test qids SEM ranking: {sorted(missing)[:5]}...")
    for qid, dids in per_q.items():
        if len(dids) > 100:
            errors.append(f"query {qid}: {len(dids)} docs (>100)")
        if len(set(dids)) != len(dids):
            errors.append(f"query {qid}: docids duplicados")
        for d in dids:
            if not (d.isdigit() and len(d) == 7):
                errors.append(f"query {qid}: EntityId suspeito {d!r}")
                break

    ids_path = CACHE / "ids.npy"
    if ids_path.exists():
        import numpy as np

        valid = set(np.load(ids_path).astype("U").tolist())
        bad = {d for dids in per_q.values() for d in dids if d not in valid}
        if bad:
            errors.append(f"{len(bad)} EntityIds inexistentes no corpus: {sorted(bad)[:5]}...")

    total = sum(len(v) for v in per_q.values())
    if errors:
        print(f"REPROVADA ({Path(args.sub).name}):")
        for e in errors:
            print(" -", e)
        sys.exit(1)
    print(f"OK: {Path(args.sub).name} — {len(per_q)} queries, {total} linhas (max 23300).")


if __name__ == "__main__":
    main()
