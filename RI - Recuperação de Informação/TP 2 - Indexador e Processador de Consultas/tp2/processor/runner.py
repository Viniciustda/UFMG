"""Orquestração do processor: lê queries, abre o índice, ranqueia, imprime."""

from __future__ import annotations

import json
import time
from pathlib import Path

from tp2 import text
from tp2.config import TOP_K

from .daat import daat_conjunctive
from .parallel import default_n_threads, run_queries_parallel
from .ranker import make_ranker
from .stats import QueryMetricsCollector, QueryStat
from .store import IndexHandle, open_index_ctx


def _load_queries(path: str | Path) -> list[str]:
    out: list[str] = []
    with open(path, "r", encoding="utf-8", newline=None) as fh:
        for line in fh:
            q = line.strip()
            if q:
                out.append(q)
    return out


def _process_one(query: str, idx: IndexHandle, ranker, collector) -> dict:
    t0 = time.perf_counter()
    stems = list(text.analyze(query))
    top, n_matched = daat_conjunctive(stems, idx, ranker, k=TOP_K)
    elapsed = (time.perf_counter() - t0) * 1000.0

    results = [
        {"ID": idx.ext_id(did), "Score": round(score, 4)}
        for score, did in top
    ]
    if collector is not None:
        collector.record(
            QueryStat(
                query=query,
                n_stems=len(stems),
                n_matched=n_matched,
                n_returned=len(top),
                elapsed_ms=elapsed,
                scores=[s for s, _ in top],
            )
        )
    return {"Query": query, "Results": results}


def run(index_dir: str, queries_path: str, ranker_name: str) -> int:
    queries = _load_queries(queries_path)
    if not queries:
        return 0

    ranker = make_ranker(ranker_name)
    collector = QueryMetricsCollector()

    with open_index_ctx(index_dir) as idx:
        def fn(q: str) -> dict:
            return _process_one(q, idx, ranker, collector)

        n_threads = default_n_threads()
        run_queries_parallel(queries, fn, n_threads=n_threads)

        try:
            collector.write(
                Path(index_dir) / f"query_metrics_{ranker_name.lower()}.json",
                ranker_name,
            )
        except OSError:
            pass

    return 0
