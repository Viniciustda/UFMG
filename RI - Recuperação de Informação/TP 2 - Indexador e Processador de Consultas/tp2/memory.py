"""Monitor de memória do processo principal.

O indexador usa threads (não processos filhos), então o RSS do processo
principal cobre toda a memória da implementação.

O RSS é usado apenas como teto de segurança e para relatório — o gatilho
de flush principal é a contagem de postings do acumulador (ver runner.py),
que é determinística e não depende de o SO devolver o heap após free().
"""

from __future__ import annotations

import os
from dataclasses import dataclass, field

import psutil


_BYTES_PER_MB: int = 1024 * 1024


@dataclass(slots=True)
class MemoryGuard:
    budget_mb: int
    process: psutil.Process = field(default_factory=lambda: psutil.Process(os.getpid()))

    def rss_bytes(self) -> int:
        try:
            return self.process.memory_info().rss
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            return 0

    def rss_mb(self) -> float:
        return self.rss_bytes() / _BYTES_PER_MB

    def is_over_budget(self) -> bool:
        return self.rss_mb() > self.budget_mb
