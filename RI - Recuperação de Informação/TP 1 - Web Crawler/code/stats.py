"""
Stats module — crawl metrics collection.

Tracks crawl progress and collects data for the technical report:
- Pages crawled (used for stop condition)
- Pages per domain
- Tokens per page
- Download rate over time
"""

import json
import threading
import time
from urllib.parse import urlparse


class Stats:
    """Thread-safe metrics collector."""

    def __init__(self):
        self._lock = threading.Lock()
        self._pages_crawled = 0
        self._pages_per_domain: dict[str, int] = {}
        self._tokens_per_page: list[int] = []
        self._bytes_downloaded = 0
        self._timestamps: list[float] = []
        self._start_time: float = time.time()

    @property
    def pages_crawled(self) -> int:
        with self._lock:
            return self._pages_crawled

    def try_claim(self, limit: int) -> bool:
        """
        Claim a page slot if under the limit.

        Returns True if the slot was claimed (caller should store the page),
        False if the limit has already been reached (caller should discard).
        """
        with self._lock:
            if self._pages_crawled >= limit:
                return False
            self._pages_crawled += 1
            return True

    def record_page(self, url: str, num_tokens: int, num_bytes: int) -> None:
        """Record metadata for a successfully crawled page."""
        host = urlparse(url).hostname or ""

        with self._lock:
            self._pages_per_domain[host] = (
                self._pages_per_domain.get(host, 0) + 1
            )
            self._tokens_per_page.append(num_tokens)
            self._bytes_downloaded += num_bytes
            self._timestamps.append(time.time())

    def dump_report(self, filepath: str) -> None:
        """Export all collected metrics to a JSON file."""
        with self._lock:
            elapsed = time.time() - self._start_time
            data = {
                "total_pages": self._pages_crawled,
                "total_bytes": self._bytes_downloaded,
                "elapsed_seconds": elapsed,
                "pages_per_second": (
                    self._pages_crawled / elapsed if elapsed > 0 else 0
                ),
                "unique_domains": len(self._pages_per_domain),
                "pages_per_domain": dict(
                    sorted(
                        self._pages_per_domain.items(),
                        key=lambda x: x[1],
                        reverse=True,
                    )
                ),
                "tokens_per_page": {
                    "count": len(self._tokens_per_page),
                    "min": min(self._tokens_per_page) if self._tokens_per_page else 0,
                    "max": max(self._tokens_per_page) if self._tokens_per_page else 0,
                    "mean": (
                        sum(self._tokens_per_page) / len(self._tokens_per_page)
                        if self._tokens_per_page
                        else 0
                    ),
                    "values": list(self._tokens_per_page),
                },
                "timestamps": self._timestamps,
            }

        with open(filepath, "w", encoding="utf-8") as f:
            json.dump(data, f, indent=2, ensure_ascii=False)