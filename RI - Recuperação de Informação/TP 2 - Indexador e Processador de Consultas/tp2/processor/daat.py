"""DAAT conjuntivo + top-k via min-heap."""

from __future__ import annotations

import heapq

from tp2.config import LexEntry, TOP_K

from .postings import PostingIterator
from .ranker import Ranker
from .store import IndexHandle


def daat_conjunctive(
    stems: list[str],
    idx: IndexHandle,
    ranker: Ranker,
    k: int = TOP_K,
) -> tuple[list[tuple[float, int]], int]:
    """Retorna (top-k docs ordenados por score desc, nº total de docs casados).

    O segundo valor conta todos os documentos que satisfazem a conjunção
    inteira --- sem truncar em ``k`` ---, número exigido pela caracterização."""
    if not stems:
        return [], 0

    # Qualquer stem ausente do lexicon ⇒ conjunção impossível.
    entries: list[LexEntry] = []
    for s in stems:
        e = idx.lex.get(s)
        if e is None:
            return [], 0
        entries.append(e)

    # Lista mais rara como leader: reduz o número de pivots avaliados.
    entries.sort(key=lambda e: e.df)
    iters = [PostingIterator(idx.inv_mmap, e) for e in entries]
    currs = [next(it, None) for it in iters]
    if any(c is None for c in currs):
        return [], 0

    dfs = [e.df for e in entries]
    heap: list[tuple[float, int]] = []
    n_matched = 0  # docs que satisfazem a conjunção inteira (sem truncar em k)

    while currs[0] is not None:
        target = currs[0].docid
        all_match = True

        for i in range(1, len(currs)):
            ci = currs[i]
            while ci is not None and ci.docid < target:
                ci = next(iters[i], None)
            currs[i] = ci
            if ci is None:
                return _heap_to_sorted(heap), n_matched
            if ci.docid != target:
                all_match = False
                break

        if all_match:
            n_matched += 1
            tfs = [c.tf for c in currs]  # type: ignore[union-attr]
            score = ranker.score(target, tfs, dfs, idx)
            if len(heap) < k:
                heapq.heappush(heap, (score, target))
            else:
                heapq.heappushpop(heap, (score, target))
            for i in range(len(currs)):
                currs[i] = next(iters[i], None)
            if currs[0] is None:
                break
        else:
            currs[0] = next(iters[0], None)

    return _heap_to_sorted(heap), n_matched


def _heap_to_sorted(heap: list[tuple[float, int]]) -> list[tuple[float, int]]:
    # Empates de score quebrados por docid asc.
    return sorted(heap, key=lambda x: (-x[0], x[1]))
