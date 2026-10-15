"""Métricas por consulta (para o relatório)."""

from __future__ import annotations

import json
import statistics
from dataclasses import dataclass, field
from pathlib import Path
from threading import Lock


@dataclass(slots=True)
class QueryStat:
    query: str
    n_stems: int
    n_matched: int
    n_returned: int
    elapsed_ms: float
    scores: list[float] = field(default_factory=list)

    def summary(self) -> dict:
        out = {
            "query": self.query,
            "n_stems": self.n_stems,
            "n_matched": self.n_matched,
            "n_returned": self.n_returned,
            "elapsed_ms": round(self.elapsed_ms, 2),
        }
        if self.scores:
            out["score_min"] = round(min(self.scores), 4)
            out["score_max"] = round(max(self.scores), 4)
            out["score_median"] = round(statistics.median(self.scores), 4)
        return out


class QueryMetricsCollector:
    """Thread-safe (várias threads no parallel.py)."""

    def __init__(self) -> None:
        self._stats: list[QueryStat] = []
        self._lock = Lock()

    def record(self, stat: QueryStat) -> None:
        with self._lock:
            self._stats.append(stat)

    def write(self, path: Path, ranker_name: str) -> None:
        out = {
            "ranker": ranker_name,
            "queries": [s.summary() for s in self._stats],
        }
        with open(path, "w", encoding="utf-8") as fh:
            json.dump(out, fh, indent=2, ensure_ascii=False)
