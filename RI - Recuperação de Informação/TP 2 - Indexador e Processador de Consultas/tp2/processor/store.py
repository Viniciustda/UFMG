"""Abre o índice (mmap do inverted_index e doc_index, lexicon em RAM)."""

from __future__ import annotations

import json
import mmap
import struct
from contextlib import contextmanager
from dataclasses import dataclass, field
from pathlib import Path

from tp2.config import (
    BM25_B,
    BM25_K1,
    CORPUS_STATS_FILE,
    DOC_INDEX_FILE,
    DOC_INDEX_HEADER_SIZE,
    DOC_RECORD_SIZE,
    EXT_ID_MAX_LEN,
    INV_INDEX_FILE,
    LEXICON_FILE,
    LexEntry,
    MAGIC_DOC_INDEX,
)


_DOC_RECORD = struct.Struct(f"<{EXT_ID_MAX_LEN}sII")
_DOC_HEADER = struct.Struct("<4sId")


@dataclass(slots=True)
class IndexHandle:
    inv_mmap: mmap.mmap
    inv_file_handle: object
    doc_mmap: mmap.mmap
    doc_file_handle: object
    lex: dict[str, LexEntry]
    n_docs: int
    avdl: float
    n_unique_terms: int
    total_tokens: int
    k1: float
    b_param: float
    _ext_id_cache: dict[int, str] = field(default_factory=dict)

    def doc_length(self, docid: int) -> int:
        offset = DOC_INDEX_HEADER_SIZE + docid * DOC_RECORD_SIZE
        _, length, _ = _DOC_RECORD.unpack_from(self.doc_mmap, offset)
        return length

    def ext_id(self, docid: int) -> str:
        cached = self._ext_id_cache.get(docid)
        if cached is not None:
            return cached
        offset = DOC_INDEX_HEADER_SIZE + docid * DOC_RECORD_SIZE
        raw, _, _ = _DOC_RECORD.unpack_from(self.doc_mmap, offset)
        ext = raw.rstrip(b"\x00").decode("ascii", errors="replace")
        self._ext_id_cache[docid] = ext
        return ext

    def close(self) -> None:
        for obj in (self.inv_mmap, self.doc_mmap,
                    self.inv_file_handle, self.doc_file_handle):
            try:
                obj.close()  
            except Exception: 
                pass


def _load_corpus_stats(path: Path) -> dict:
    if not path.is_file():
        return {}
    try:
        with open(path, "r", encoding="utf-8") as fh:
            return json.load(fh)
    except (json.JSONDecodeError, OSError):
        return {}


def open_index(index_dir: str | Path) -> IndexHandle:
    base = Path(index_dir)
    if not base.is_dir():
        raise FileNotFoundError(f"diretório do índice não encontrado: {base}")

    inv_path = base / INV_INDEX_FILE
    lex_path = base / LEXICON_FILE
    doc_path = base / DOC_INDEX_FILE
    stats_path = base / CORPUS_STATS_FILE

    if not inv_path.is_file() or not lex_path.is_file() or not doc_path.is_file():
        raise FileNotFoundError(
            f"índice incompleto em {base}: esperado {INV_INDEX_FILE}, "
            f"{LEXICON_FILE}, {DOC_INDEX_FILE}"
        )

    inv_fh = open(inv_path, "rb")
    if inv_path.stat().st_size > 0:
        inv_mm = mmap.mmap(inv_fh.fileno(), 0, access=mmap.ACCESS_READ)
    else:
        # mmap não aceita arquivo vazio: usa buffer sentinela.
        inv_mm = mmap.mmap(-1, 1)

    from tp2.indexer.merge import load_lexicon
    lex = load_lexicon(lex_path)

    doc_fh = open(doc_path, "rb")
    doc_mm = mmap.mmap(doc_fh.fileno(), 0, access=mmap.ACCESS_READ)
    magic, n_docs, avdl = _DOC_HEADER.unpack_from(doc_mm, 0)
    if magic != MAGIC_DOC_INDEX:
        doc_mm.close()
        doc_fh.close()
        raise ValueError(
            f"magic incorreto em {doc_path}: {magic!r}, esperado {MAGIC_DOC_INDEX!r}"
        )

    cs = _load_corpus_stats(stats_path)
    k1 = float(cs.get("k1", BM25_K1))
    b_param = float(cs.get("b", BM25_B))
    n_unique_terms = int(cs.get("n_unique_terms", len(lex)))
    total_tokens = int(cs.get("total_tokens", 0))

    return IndexHandle(
        inv_mmap=inv_mm,
        inv_file_handle=inv_fh,
        doc_mmap=doc_mm,
        doc_file_handle=doc_fh,
        lex=lex,
        n_docs=int(n_docs),
        avdl=float(avdl),
        n_unique_terms=n_unique_terms,
        total_tokens=total_tokens,
        k1=k1,
        b_param=b_param,
    )


@contextmanager
def open_index_ctx(index_dir: str | Path):
    h = open_index(index_dir)
    try:
        yield h
    finally:
        h.close()
