"""Orquestração do indexador: reader → threads → SPIMI → merge → stats."""

from __future__ import annotations

import json
import shutil
import struct
import sys
import time
from pathlib import Path

from tp2.config import (
    BM25_B,
    BM25_K1,
    BUFFER_RATIO,
    CORPUS_STATS_FILE,
    DOC_INDEX_FILE,
    DOC_INDEX_HEADER_SIZE,
    DOC_RECORD_SIZE,
    DOCS_PER_BATCH,
    EST_BYTES_PER_POSTING,
    EXT_ID_MAX_LEN,
    MAGIC_DOC_INDEX,
    MEM_CHECK_INTERVAL,
    RUNS_DIR,
)
from tp2.memory import MemoryGuard

from .builder import MemoryIndexBuilder, cleanup_partial_runs
from .merge import merge_runs
from .reader import batched, iter_documents
from .stats import DfHistogram, IndexerMetrics
from .workers import map_batches


# 16 bytes ext_id (NUL-padded) + u32 doc_length + u32 reserved = 24 bytes/registro.
_DOC_RECORD = struct.Struct(f"<{EXT_ID_MAX_LEN}sII")
# Header: magic(4) + n_docs(u32) + avdl(f64) = 16 bytes.
_DOC_HEADER = struct.Struct("<4sId")


def _open_doc_index(index_dir: Path):
    p = index_dir / DOC_INDEX_FILE
    fh = open(p, "wb")
    fh.write(b"\x00" * DOC_INDEX_HEADER_SIZE)
    return fh


def _write_doc_record(fh, ext_id: str, doc_length: int) -> None:
    raw = ext_id.encode("ascii", errors="ignore")[:EXT_ID_MAX_LEN]
    raw = raw.ljust(EXT_ID_MAX_LEN, b"\x00")
    fh.write(_DOC_RECORD.pack(raw, doc_length, 0))


def _finalize_doc_index(fh, index_dir: Path, n_docs: int, avdl: float) -> None:
    fh.flush()
    fh.seek(0)
    fh.write(_DOC_HEADER.pack(MAGIC_DOC_INDEX, n_docs, avdl))
    fh.close()


def _write_corpus_stats(
    index_dir: Path,
    n_docs: int,
    n_unique_terms: int,
    total_tokens: int,
    avdl: float,
) -> None:
    out = {
        "n_docs": n_docs,
        "n_unique_terms": n_unique_terms,
        "total_tokens": total_tokens,
        "avdl": round(avdl, 4),
        "k1": BM25_K1,
        "b": BM25_B,
    }
    with open(index_dir / CORPUS_STATS_FILE, "w", encoding="utf-8") as fh:
        json.dump(out, fh, indent=2, ensure_ascii=False)


def run(memory_mb: int, corpus_path: str, index_dir: str) -> int:
    t_start = time.perf_counter()

    idx_path = Path(index_dir)
    idx_path.mkdir(parents=True, exist_ok=True)
    runs_dir = idx_path / RUNS_DIR
    cleanup_partial_runs(runs_dir)

    metrics = IndexerMetrics()
    guard = MemoryGuard(budget_mb=memory_mb)

    # Gatilho de flush: número de postings que cabe na fração BUFFER_RATIO do orçamento. Determinístico e portável — ver EST_BYTES_PER_POSTING.
    flush_threshold = max(
        1,
        int(memory_mb * BUFFER_RATIO * 1024 * 1024 / EST_BYTES_PER_POSTING),
    )
    # Teto de segurança em RSS: se o uso real se aproximar do orçamento, força flush mesmo antes do limiar de postings.
    rss_ceiling_mb = memory_mb * 0.9

    print(
        f"[indexer] orçamento: {memory_mb} MB "
        f"(flush a cada ~{flush_threshold:,} postings)",
        file=sys.stderr, flush=True,
    )

    builder = MemoryIndexBuilder()
    next_did = 0
    total_tokens = 0
    n_docs = 0

    doc_index_fh = _open_doc_index(idx_path)

    # Fase 1: ler, pré-processar em threads, acumular postings em memória.
    t_phase1 = time.perf_counter()

    def _batches():
        nonlocal next_did
        for batch in batched(iter_documents(corpus_path), DOCS_PER_BATCH):
            yield (next_did, batch)
            next_did += len(batch)

    docs_since_check = 0
    for start_did, results in map_batches(_batches()):
        for local_id, ext_id, doc_len, term_tfs in results:
            global_did = start_did + local_id
            _write_doc_record(doc_index_fh, ext_id, doc_len)
            if term_tfs:
                builder.add(global_did, term_tfs)
            total_tokens += doc_len
            n_docs += 1
            docs_since_check += 1

        if docs_since_check >= MEM_CHECK_INTERVAL:
            docs_since_check = 0
            over_postings = builder.n_postings >= flush_threshold
            over_rss = guard.rss_mb() > rss_ceiling_mb
            if (over_postings or over_rss) and not builder.is_empty():
                t_flush = time.perf_counter()
                runs_so_far = len(builder.runs_emitted)
                builder.flush(runs_dir)
                metrics.record("flush", time.perf_counter() - t_flush)
                print(
                    f"[indexer] flush #{runs_so_far+1} after {n_docs} docs, "
                    f"RSS={guard.rss_mb():.0f}MB",
                    file=sys.stderr, flush=True,
                )

    if not builder.is_empty():
        t_flush = time.perf_counter()
        builder.flush(runs_dir)
        metrics.record("flush", time.perf_counter() - t_flush)

    metrics.record("phase1_read_preprocess", time.perf_counter() - t_phase1)
    metrics.n_docs = n_docs
    metrics.total_tokens = total_tokens
    metrics.n_runs = len(builder.runs_emitted)

    avdl = (total_tokens / n_docs) if n_docs > 0 else 0.0
    metrics.avdl = avdl

    _finalize_doc_index(doc_index_fh, idx_path, n_docs, avdl)
    print(
        f"[indexer] fase 1 concluída: {n_docs} docs, {metrics.n_runs} runs, "
        f"avdl={avdl:.1f}",
        file=sys.stderr, flush=True,
    )

    # Fase 2: merge externo dos runs → inverted_index.bin + lexicon.bin.
    t_phase2 = time.perf_counter()
    df_histogram = DfHistogram()
    if not builder.runs_emitted:
        # Corpus vazio: gera lexicon válido com 0 termos.
        from tp2.config import LEXICON_FILE, MAGIC_LEXICON
        lex_path = idx_path / LEXICON_FILE
        with open(lex_path, "wb") as fh:
            fh.write(struct.pack("<4sIQ", MAGIC_LEXICON, 0, 0))
        (idx_path / "inverted_index.bin").touch()
        num_lists, total_postings = 0, 0
    else:
        num_lists, total_postings = merge_runs(
            builder.runs_emitted, idx_path, guard=guard, df_histogram=df_histogram
        )
    metrics.df_histogram = df_histogram
    metrics.record("phase2_merge", time.perf_counter() - t_phase2)
    print(
        f"[indexer] fase 2 concluída: {num_lists} listas, {total_postings} postings",
        file=sys.stderr, flush=True,
    )

    # Fase 3: limpeza dos runs + stats auxiliares.
    t_phase3 = time.perf_counter()
    if runs_dir.is_dir():
        shutil.rmtree(runs_dir, ignore_errors=True)

    _write_corpus_stats(idx_path, n_docs, num_lists, total_tokens, avdl)
    metrics.record("phase3_finalize", time.perf_counter() - t_phase3)

    elapsed_total = time.perf_counter() - t_start
    out_json = metrics.finalize(idx_path, elapsed_total, num_lists, total_postings)
    metrics.write_runtime_metrics(idx_path)
    metrics.write_df_histogram(idx_path)

    print(json.dumps(out_json))
    return 0
