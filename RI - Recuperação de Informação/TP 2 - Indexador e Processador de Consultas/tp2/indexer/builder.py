"""Acumulador SPIMI em memória + flush em disco como run files."""

from __future__ import annotations

import struct
from pathlib import Path
from typing import Iterator

from tp2.config import Posting


# Formato de cada termo num run (little-endian):
#   <H term_len> <utf-8 term> <I n_postings> [<I docid> <H tf>]*n_postings
_TERM_HEADER = struct.Struct("<H")
_POSTINGS_HEADER = struct.Struct("<I")
_POSTING = struct.Struct("<IH")


class MemoryIndexBuilder:
    """dict[term, list[Posting]] com flush em disco."""

    __slots__ = ("index", "n_postings", "n_docs_added", "runs_emitted")

    def __init__(self) -> None:
        self.index: dict[str, list[Posting]] = {}
        self.n_postings: int = 0
        self.n_docs_added: int = 0
        self.runs_emitted: list[Path] = []

    def add(self, did: int, term_tfs: dict[str, int]) -> None:
        idx = self.index
        for term, tf in term_tfs.items():
            lst = idx.get(term)
            if lst is None:
                idx[term] = [Posting(did, tf)]
            else:
                lst.append(Posting(did, tf))
        self.n_postings += len(term_tfs)
        self.n_docs_added += 1

    def __len__(self) -> int:
        return self.n_postings

    def is_empty(self) -> bool:
        return not self.index

    def clear(self) -> None:
        self.index.clear()
        self.n_postings = 0

    def flush(self, runs_dir: Path) -> Path | None:
        """Escreve o conteúdo num run file ordenado por termo e zera o dict."""
        if self.is_empty():
            return None

        runs_dir.mkdir(parents=True, exist_ok=True)
        k = len(self.runs_emitted)
        final_path = runs_dir / f"run_{k:05d}.tmp"
        partial_path = runs_dir / f"run_{k:05d}.tmp.partial"

        # Termos ordenados lex pra alimentar o merge com input já sorted.
        sorted_terms = sorted(self.index.keys())

        with open(partial_path, "wb") as fh:
            for term in sorted_terms:
                postings = self.index[term]
                term_bytes = term.encode("utf-8")
                fh.write(_TERM_HEADER.pack(len(term_bytes)))
                fh.write(term_bytes)
                fh.write(_POSTINGS_HEADER.pack(len(postings)))
                for p in postings:
                    fh.write(_POSTING.pack(p.docid, min(p.tf, 0xFFFF)))

        # Rename: se cair no meio sobra só o .partial (ignorado).
        partial_path.replace(final_path)

        self.runs_emitted.append(final_path)
        self.clear()
        return final_path


def iter_run(run_path: Path) -> Iterator[tuple[str, list[Posting]]]:
    """Lê um run file em streaming, yielding (term, postings)."""
    with open(run_path, "rb") as fh:
        while True:
            header = fh.read(_TERM_HEADER.size)
            if not header:
                return
            (term_len,) = _TERM_HEADER.unpack(header)
            term = fh.read(term_len).decode("utf-8")
            (n_postings,) = _POSTINGS_HEADER.unpack(fh.read(_POSTINGS_HEADER.size))
            postings: list[Posting] = [None] * n_postings  # type: ignore[list-item]
            for i in range(n_postings):
                docid, tf = _POSTING.unpack(fh.read(_POSTING.size))
                postings[i] = Posting(docid, tf)
            yield term, postings


def cleanup_partial_runs(runs_dir: Path) -> None:
    """Remove .partial órfãos de uma execução anterior."""
    if not runs_dir.is_dir():
        return
    for p in runs_dir.glob("*.tmp.partial"):
        try:
            p.unlink()
        except OSError:
            pass
