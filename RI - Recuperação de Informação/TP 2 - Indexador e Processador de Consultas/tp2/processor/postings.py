"""Iterador lazy sobre uma posting list comprimida do inverted_index."""

from __future__ import annotations

from tp2.codec import BitReader
from tp2.config import LexEntry, Posting


class PostingIterator:
    __slots__ = ("_br", "_remaining", "_prev_docid_plus1")

    def __init__(self, mmap_obj, entry: LexEntry) -> None:
        self._br = BitReader(mmap_obj, entry.offset, entry.length)
        self._remaining = self._br.read_gamma()
        self._prev_docid_plus1 = 0

    def __iter__(self):
        return self

    def __next__(self) -> Posting:
        if self._remaining <= 0:
            raise StopIteration

        gap = self._br.read_gamma()
        docid_plus1 = (
            gap if self._prev_docid_plus1 == 0 else self._prev_docid_plus1 + gap
        )
        self._prev_docid_plus1 = docid_plus1

        marker = self._br.read_bit()
        if marker == 0:
            tf = self._br.read_unary() + 1
        else:
            tf = self._br.read_gamma()

        self._remaining -= 1
        return Posting(docid_plus1 - 1, tf)
