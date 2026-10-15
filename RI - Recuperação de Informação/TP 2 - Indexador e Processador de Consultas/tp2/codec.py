"""Codec γ + unário para posting lists.

Layout de uma lista:
    γ(df)
    γ(d_1)                              # primeiro docid sem gap
    marker + (unary(tf-1) | γ(tf))      # marker 0 = unário, 1 = γ
    para i = 2..df: γ(d_i - d_{i-1}), marker, tf
    padding zero até byte boundary
"""

from __future__ import annotations

from typing import Iterable, Iterator

from tp2.config import Posting, TF_UNARY_THRESHOLD


class BitWriter:
    """Acumula bits e devolve bytes."""

    __slots__ = ("_buf", "_nbits", "_chunks")

    def __init__(self) -> None:
        self._buf: int = 0
        self._nbits: int = 0
        self._chunks: list[bytes] = []

    def write_bit(self, bit: int) -> None:
        self._buf = (self._buf << 1) | (bit & 1)
        self._nbits += 1
        self._flush_full_bytes()

    def write_bits(self, value: int, nbits: int) -> None:
        if nbits <= 0:
            return
        mask = (1 << nbits) - 1
        self._buf = (self._buf << nbits) | (value & mask)
        self._nbits += nbits
        self._flush_full_bytes()

    def write_unary(self, n: int) -> None:
        if n < 0:
            raise ValueError(f"unary requires n ≥ 0, got {n}")
        self._buf = (self._buf << (n + 1)) | 1
        self._nbits += n + 1
        self._flush_full_bytes()

    def write_gamma(self, n: int) -> None:
        if n < 1:
            raise ValueError(f"gamma requires n ≥ 1, got {n}")
        L = n.bit_length() - 1
        self.write_unary(L)
        if L > 0:
            self.write_bits(n, L)

    def _flush_full_bytes(self) -> None:
        while self._nbits >= 8:
            shift = self._nbits - 8
            byte = (self._buf >> shift) & 0xFF
            self._chunks.append(bytes((byte,)))
            self._buf &= (1 << shift) - 1
            self._nbits = shift

    def to_bytes(self) -> bytes:
        if self._nbits > 0:
            byte = (self._buf << (8 - self._nbits)) & 0xFF
            self._chunks.append(bytes((byte,)))
            self._buf = 0
            self._nbits = 0
        out = b"".join(self._chunks)
        self._chunks = []
        return out


class BitReader:
    """Lê bits MSB→LSB de uma fatia de bytes ou mmap."""

    __slots__ = ("_data", "_byte_pos", "_bit_pos", "_end")

    def __init__(self, data, start: int = 0, length: int | None = None) -> None:
        self._data = data
        self._byte_pos: int = start
        self._bit_pos: int = 0
        self._end: int = start + length if length is not None else len(data)

    def read_bit(self) -> int:
        if self._byte_pos >= self._end:
            raise EOFError("BitReader past end of data")
        byte = self._data[self._byte_pos]
        bit = (byte >> (7 - self._bit_pos)) & 1
        self._bit_pos += 1
        if self._bit_pos == 8:
            self._bit_pos = 0
            self._byte_pos += 1
        return bit

    def read_bits(self, nbits: int) -> int:
        value = 0
        for _ in range(nbits):
            value = (value << 1) | self.read_bit()
        return value

    def read_unary(self) -> int:
        n = 0
        while self.read_bit() == 0:
            n += 1
        return n

    def read_gamma(self) -> int:
        L = self.read_unary()
        if L == 0:
            return 1
        low = self.read_bits(L)
        return (1 << L) | low


def encode_postings(postings: list[Posting]) -> bytes:
    """Codifica uma posting list (postings em ordem asc de docid)."""
    if not postings:
        return b"\x00"

    bw = BitWriter()
    bw.write_gamma(len(postings))

    prev_docid_plus1 = 0
    for i, p in enumerate(postings):
        # γ exige n ≥ 1, docids começam em 0, então somamos 1.
        docid_plus1 = p.docid + 1
        if i == 0:
            bw.write_gamma(docid_plus1)
        else:
            gap = docid_plus1 - prev_docid_plus1
            if gap < 1:
                raise ValueError(
                    f"postings desordenadas: docid {p.docid} após {prev_docid_plus1 - 1}"
                )
            bw.write_gamma(gap)
        prev_docid_plus1 = docid_plus1

        tf = max(1, p.tf)
        if tf <= TF_UNARY_THRESHOLD:
            bw.write_bit(0)
            bw.write_unary(tf - 1)
        else:
            # tf grande: unário(500) custaria 501 bits → fallback γ.
            bw.write_bit(1)
            bw.write_gamma(tf)

    return bw.to_bytes()


def decode_postings(data, offset: int, length: int) -> Iterator[Posting]:
    """Itera sobre as postings de data[offset:offset+length]."""
    br = BitReader(data, offset, length)
    df = br.read_gamma()
    if df < 1:
        return

    prev_docid_plus1 = 0
    for i in range(df):
        if i == 0:
            docid_plus1 = br.read_gamma()
        else:
            gap = br.read_gamma()
            docid_plus1 = prev_docid_plus1 + gap
        prev_docid_plus1 = docid_plus1

        marker = br.read_bit()
        if marker == 0:
            tf = br.read_unary() + 1
        else:
            tf = br.read_gamma()

        yield Posting(docid_plus1 - 1, tf)


def round_trip(postings: Iterable[Posting]) -> list[Posting]:
    pl = list(postings)
    blob = encode_postings(pl)
    return list(decode_postings(blob, 0, len(blob)))
