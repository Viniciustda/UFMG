"""Constantes e estruturas compartilhadas entre indexer e processor."""

from __future__ import annotations

from dataclasses import dataclass
from typing import NamedTuple


class Posting(NamedTuple):
    docid: int
    tf: int


@dataclass(slots=True)
class LexEntry:
    offset: int
    length: int
    df: int
    ctf: int


@dataclass(slots=True)
class DocEntry:
    ext_id: str
    length: int


# Pré-processamento
MIN_TOKEN_LEN: int = 3
STEM_CACHE_SIZE: int = 200_000

# Indexer
BUFFER_RATIO: float = 0.50          # fração de -m reservada ao acumulador
DOCS_PER_BATCH: int = 2048
MEM_CHECK_INTERVAL: int = 500

# Serve para derivar o gatilho de flush a partir de -m, sem depender de leitura de RSS (que no Linux fica presa no high-water mark porque o glibc retém o heap após free()).
EST_BYTES_PER_POSTING: int = 110

# Processor
TOP_K: int = 10
BM25_K1: float = 1.2
BM25_B: float = 0.75
TFIDF_B: float = 0.75

# Arquivos do índice
INV_INDEX_FILE: str = "inverted_index.bin"
LEXICON_FILE: str = "lexicon.bin"
DOC_INDEX_FILE: str = "doc_index.bin"
CORPUS_STATS_FILE: str = "corpus_stats.json"
RUNTIME_METRICS_FILE: str = "runtime_metrics.json"
RUNS_DIR: str = "runs"

# Magic bytes pra detectar incompatibilidade indexer↔processor
MAGIC_LEXICON: bytes = b"LEX1"
MAGIC_DOC_INDEX: bytes = b"DOC1"

# Doc index: 16 bytes ext_id + u32 length + u32 reserved = 24 bytes/registro.
DOC_RECORD_SIZE: int = 24
DOC_INDEX_HEADER_SIZE: int = 16     # magic(4) + n_docs(4) + avdl(8)
EXT_ID_MAX_LEN: int = 16

# Acima deste limiar, codifica tf com γ em vez de unário.
TF_UNARY_THRESHOLD: int = 32
