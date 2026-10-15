"""ThreadPoolExecutor pra processar várias consultas em paralelo."""

from __future__ import annotations

import json
import os
import sys
from concurrent.futures import ThreadPoolExecutor
from typing import Callable, Iterable


def default_n_threads() -> int:
    env = os.environ.get("TP2_PROCESSOR_THREADS")
    if env:
        try:
            return max(1, int(env))
        except ValueError:
            pass
    return max(1, (os.cpu_count() or 2) - 1)


def run_queries_parallel(
    queries: Iterable[str],
    process_one: Callable[[str], dict],
    n_threads: int | None = None,
) -> None:
    """Imprime um JSON por consulta no stdout, em ordem de submissão."""
    qs = list(queries)
    if not qs:
        return
    n = n_threads if n_threads is not None else default_n_threads()
    n = min(n, len(qs))

    if n <= 1:
        for q in qs:
            print(json.dumps(process_one(q), ensure_ascii=False))
            sys.stdout.flush()
        return

    with ThreadPoolExecutor(max_workers=n) as pool:
        # Itera sobre os Futures na ordem de submit (não as_completed) pra saída bater com a ordem do arquivo de queries.
        futures = [pool.submit(process_one, q) for q in qs]
        for fut in futures:
            print(json.dumps(fut.result(), ensure_ascii=False))
            sys.stdout.flush()
