"""Pré-processamento paralelo via ThreadPoolExecutor."""

from __future__ import annotations

import os
from collections import Counter, deque
from concurrent.futures import ThreadPoolExecutor
from typing import Iterable, Iterator

from tp2 import text


def _default_n_threads() -> int:
    env = os.environ.get("TP2_INDEXER_WORKERS")
    if env:
        try:
            return max(1, int(env))
        except ValueError:
            pass
    return max(1, (os.cpu_count() or 2) - 1)


WorkerDocResult = tuple[int, str, int, dict[str, int]]


def _process_batch(
    batch: tuple[int, list[dict]],
) -> tuple[int, list[WorkerDocResult]]:
    start_did, docs = batch
    out: list[WorkerDocResult] = []
    for i, d in enumerate(docs):
        ext_id = str(d.get("id", ""))
        title = d.get("title", "") or ""
        body = d.get("text", "") or ""
        keywords = d.get("keywords", []) or []
        if isinstance(keywords, str):
            keywords = [keywords]
        joined = " ".join((title, body, " ".join(str(k) for k in keywords)))
        stems = list(text.analyze(joined))
        doc_len = len(stems)
        term_tfs = dict(Counter(stems))
        out.append((i, ext_id, doc_len, term_tfs))
    return start_did, out


def map_batches(
    batches: Iterable[tuple[int, list[dict]]],
    n_threads: int | None = None,
) -> Iterator[tuple[int, list[WorkerDocResult]]]:
    """Yield resultados na ordem de submissão.

    Com n_threads == 1, roda serial in-process (evita overhead do pool).
    Com n_threads > 1, usa janela deslizante: no máximo 2*n_threads
    batches em voo para limitar memória residente em buffers.
    """
    if n_threads is None:
        n_threads = _default_n_threads()

    text.warm_up()

    if n_threads == 1:
        for batch in batches:
            yield _process_batch(batch)
        return

    with ThreadPoolExecutor(max_workers=n_threads) as pool:
        max_inflight = n_threads * 2
        in_flight: deque = deque()
        for batch in batches:
            in_flight.append(pool.submit(_process_batch, batch))
            while len(in_flight) >= max_inflight:
                yield in_flight.popleft().result()
        while in_flight:
            yield in_flight.popleft().result()
