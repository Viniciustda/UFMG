"""TFIDF (pivoted length normalization) e BM25 (Okapi)."""

from __future__ import annotations

import math
from typing import Protocol

from tp2.config import BM25_B, BM25_K1, TFIDF_B


class Ranker(Protocol):
    def score(self, docid: int, tfs: list[int], dfs: list[int], idx) -> float: ...


class TFIDF:
    """Pivoted length normalization VSM"""

    __slots__ = ("b",)

    def __init__(self, b: float = TFIDF_B) -> None:
        self.b = b

    def score(self, docid: int, tfs: list[int], dfs: list[int], idx) -> float:
        N = idx.n_docs
        avdl = idx.avdl if idx.avdl > 0 else 1.0
        dl = idx.doc_length(docid)
        norm = (1.0 - self.b) + self.b * (dl / avdl)
        if norm <= 0:
            norm = 1.0
        s = 0.0
        for tf, df in zip(tfs, dfs):
            if tf <= 0 or df <= 0:
                continue
            tf_norm = math.log(1.0 + math.log(1.0 + tf)) / norm
            idf = math.log((N + 1) / df)
            s += tf_norm * idf
        return s


class BM25:
    """Okapi BM25"""

    __slots__ = ("k1", "b")

    def __init__(self, k1: float = BM25_K1, b: float = BM25_B) -> None:
        self.k1 = k1
        self.b = b

    def score(self, docid: int, tfs: list[int], dfs: list[int], idx) -> float:
        N = idx.n_docs
        avdl = idx.avdl if idx.avdl > 0 else 1.0
        dl = idx.doc_length(docid)
        len_norm = self.k1 * ((1.0 - self.b) + self.b * (dl / avdl))
        s = 0.0
        for tf, df in zip(tfs, dfs):
            if tf <= 0 or df <= 0:
                continue
            tf_component = (tf * (self.k1 + 1.0)) / (tf + len_norm)
            idf = math.log((N + 1) / df)
            s += tf_component * idf
        return s


def make_ranker(name: str) -> Ranker:
    n = name.upper()
    if n == "TFIDF":
        return TFIDF()
    if n == "BM25":
        return BM25()
    raise ValueError(f"Ranker desconhecido: {name!r}")
