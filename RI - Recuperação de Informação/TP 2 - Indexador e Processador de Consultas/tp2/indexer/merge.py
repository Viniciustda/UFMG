"""Merge externo k-way dos run files + escrita do índice final."""

from __future__ import annotations

import heapq
import struct
import sys
from pathlib import Path
from typing import Iterable, Iterator

from tp2.codec import encode_postings
from tp2.config import (
    INV_INDEX_FILE,
    LEXICON_FILE,
    LexEntry,
    MAGIC_LEXICON,
    Posting,
)

from .builder import iter_run


_MERGE_MEM_CHECK_INTERVAL: int = 1000

# Header do lexicon: magic(4) + n_terms(u32) + total_postings(u64) = 16 bytes.
_LEX_HEADER = struct.Struct("<4sIQ")
_LEX_ENTRY_FIXED = struct.Struct("<QIIQ")  # offset, length, df, ctf
_LEX_TERM_HEADER = struct.Struct("<H")


def merge_runs(
    run_paths: list[Path],
    index_dir: Path,
    guard: "MemoryGuard | None" = None,
    df_histogram: "DfHistogram | None" = None,
) -> tuple[int, int]:
    """Olha todos os runs e produz inverted_index.bin + lexicon.bin."""
    # Imports locais pra quebrar ciclo (runner.py importa este e memory/stats).
    from tp2.memory import MemoryGuard  
    from .stats import DfHistogram  

    inv_path = index_dir / INV_INDEX_FILE
    lex_path = index_dir / LEXICON_FILE

    iters: list[Iterator[tuple[str, list[Posting]]]] = [iter_run(p) for p in run_paths]
    heap: list[tuple[bytes, int, str, list[Posting]]] = []
    for k, it in enumerate(iters):
        try:
            term, postings = next(it)
        except StopIteration:
            continue
        heap.append((term.encode("utf-8"), k, term, postings))
    heapq.heapify(heap)

    num_lists = 0
    total_postings = 0

    # Lexicon escrito num temporário sem header, o header com n_terms vem no fim.
    lex_partial = lex_path.with_suffix(".bin.partial")

    with open(inv_path, "wb") as inv_fh, open(lex_partial, "wb") as lex_fh:
        lex_fh.write(b"\x00" * _LEX_HEADER.size)

        while heap:
            current_term_bytes = heap[0][0]

            # Olha todos os runs que contêm o termo atual, a concatenação mantém ordem global de docid porque runs sucessivos têm intervalos disjuntos de docid (garantido pelo runner).
            merged: list[Posting] = []
            while heap and heap[0][0] == current_term_bytes:
                _, k, _, postings = heapq.heappop(heap)
                merged.extend(postings)
                try:
                    nterm, npostings = next(iters[k])
                    heapq.heappush(
                        heap, (nterm.encode("utf-8"), k, nterm, npostings)
                    )
                except StopIteration:
                    pass

            offset = inv_fh.tell()
            blob = encode_postings(merged)
            inv_fh.write(blob)
            length = len(blob)

            df = len(merged)
            ctf = sum(p.tf for p in merged)

            lex_fh.write(_LEX_TERM_HEADER.pack(len(current_term_bytes)))
            lex_fh.write(current_term_bytes)
            lex_fh.write(_LEX_ENTRY_FIXED.pack(offset, length, df, ctf))

            num_lists += 1
            total_postings += df

            if df_histogram is not None:
                df_histogram.add(current_term_bytes.decode("utf-8"), df)

            # Aqui não há flush possível (fase 2). Só logamos excursões.
            if guard is not None and num_lists % _MERGE_MEM_CHECK_INTERVAL == 0:
                if guard.is_over_budget():
                    rss = guard.rss_mb()
                    print(
                        f"[merge] AVISO: RSS={rss:.0f}MB > budget="
                        f"{guard.budget_mb}MB (termo #{num_lists}, df={df}).",
                        file=sys.stderr, flush=True,
                    )

        lex_fh.seek(0)
        lex_fh.write(_LEX_HEADER.pack(MAGIC_LEXICON, num_lists, total_postings))

    lex_partial.replace(lex_path)
    return num_lists, total_postings


def load_lexicon(lex_path: Path) -> dict[str, LexEntry]:
    """Carrega o lexicon inteiro num dict (usado pelo processor)."""
    with open(lex_path, "rb") as fh:
        header = fh.read(_LEX_HEADER.size)
        magic, n_terms, _total = _LEX_HEADER.unpack(header)
        if magic != MAGIC_LEXICON:
            raise ValueError(
                f"magic incorreto em {lex_path}: {magic!r}, esperado {MAGIC_LEXICON!r}"
            )
        out: dict[str, LexEntry] = {}
        for _ in range(n_terms):
            (term_len,) = _LEX_TERM_HEADER.unpack(fh.read(_LEX_TERM_HEADER.size))
            term = fh.read(term_len).decode("utf-8")
            offset, length, df, ctf = _LEX_ENTRY_FIXED.unpack(
                fh.read(_LEX_ENTRY_FIXED.size)
            )
            out[term] = LexEntry(offset=offset, length=length, df=df, ctf=ctf)
    return out
