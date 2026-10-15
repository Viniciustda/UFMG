"""Métricas do indexador: stdout JSON da spec + arquivos auxiliares."""

from __future__ import annotations

import heapq
import json
from dataclasses import dataclass, field
from pathlib import Path

from tp2.config import (
    DOC_INDEX_FILE,
    INV_INDEX_FILE,
    LEXICON_FILE,
)


# Buckets log2 do histograma de df.
_DF_BUCKET_EDGES: tuple[int, ...] = (
    1, 2, 4, 8, 16, 32, 64, 128, 256, 512,
    1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072,
    262144, 524288, 1048576, 2097152, 4194304,
)
_TOP_N_TERMS: int = 50


@dataclass(slots=True)
class DfHistogram:
    """Distribuição de df entre termos do índice."""

    counts: list[int] = field(default_factory=lambda: [0] * len(_DF_BUCKET_EDGES))
    n_terms: int = 0
    df_sum: int = 0
    df_max: int = 0
    df_max_term: str = ""
    _top: list[tuple[int, str]] = field(default_factory=list)

    def add(self, term: str, df: int) -> None:
        if df < 1:
            return
        self.n_terms += 1
        self.df_sum += df
        if df > self.df_max:
            self.df_max = df
            self.df_max_term = term
        # Busca linear (~20 buckets) é mais barata que bisect.
        bucket = 0
        for i, edge in enumerate(_DF_BUCKET_EDGES):
            if df >= edge:
                bucket = i
            else:
                break
        self.counts[bucket] += 1
        if len(self._top) < _TOP_N_TERMS:
            heapq.heappush(self._top, (df, term))
        elif df > self._top[0][0]:
            heapq.heapreplace(self._top, (df, term))

    def to_dict(self) -> dict:
        labels: list[str] = []
        for i, lo in enumerate(_DF_BUCKET_EDGES):
            if i + 1 < len(_DF_BUCKET_EDGES):
                hi = _DF_BUCKET_EDGES[i + 1]
                labels.append(f"[{lo}, {hi})")
            else:
                labels.append(f"[{lo}, +inf)")
        mean = (self.df_sum / self.n_terms) if self.n_terms else 0.0
        top_sorted = sorted(self._top, key=lambda x: (-x[0], x[1]))
        return {
            "n_terms": self.n_terms,
            "df_mean": round(mean, 2),
            "df_max": self.df_max,
            "df_max_term": self.df_max_term,
            "df_sum_postings": self.df_sum,
            "buckets": [
                {"range": label, "n_terms": count}
                for label, count in zip(labels, self.counts)
            ],
            "top_terms": [{"term": t, "df": df} for df, t in top_sorted],
        }


@dataclass(slots=True)
class IndexerMetrics:
    n_docs: int = 0
    n_unique_terms: int = 0
    total_postings: int = 0
    total_tokens: int = 0
    n_runs: int = 0
    avdl: float = 0.0
    total_secs: float = 0.0
    _step_secs: dict[str, float] = field(default_factory=dict)
    df_histogram: DfHistogram | None = None

    def record(self, step: str, secs: float) -> None:
        self._step_secs[step] = self._step_secs.get(step, 0.0) + secs

    def finalize(
        self,
        index_dir: Path,
        elapsed_total: float,
        n_unique_terms: int,
        total_postings: int,
    ) -> dict:
        """Devolve o JSON exigido pela spec no stdout."""
        size_bytes = 0
        for fname in (INV_INDEX_FILE, LEXICON_FILE, DOC_INDEX_FILE):
            p = index_dir / fname
            if p.is_file():
                size_bytes += p.stat().st_size
        size_mb = int(round(size_bytes / (1024 * 1024)))

        self.total_secs = elapsed_total
        self.n_unique_terms = n_unique_terms
        self.total_postings = total_postings

        avg_list_size = (
            round(total_postings / n_unique_terms, 2) if n_unique_terms else 0.0
        )
        return {
            "Index Size": size_mb,
            "Elapsed Time": int(round(elapsed_total)),
            "Number of Lists": n_unique_terms,
            "Average List Size": avg_list_size,
        }

    def write_runtime_metrics(self, index_dir: Path) -> None:
        out = {
            "n_docs": self.n_docs,
            "n_unique_terms": self.n_unique_terms,
            "total_postings": self.total_postings,
            "total_tokens": self.total_tokens,
            "n_runs": self.n_runs,
            "avdl": round(self.avdl, 2),
            "total_secs": round(self.total_secs, 2),
            "step_secs": {k: round(v, 2) for k, v in self._step_secs.items()},
        }
        with open(index_dir / "runtime_metrics.json", "w", encoding="utf-8") as fh:
            json.dump(out, fh, indent=2, ensure_ascii=False)

    def write_df_histogram(self, index_dir: Path) -> None:
        if self.df_histogram is None:
            return
        with open(index_dir / "df_histogram.json", "w", encoding="utf-8") as fh:
            json.dump(self.df_histogram.to_dict(), fh, indent=2, ensure_ascii=False)
