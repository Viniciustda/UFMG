"""
Frontier module — domain-aware URL queue with round-robin scheduling.

Manages the queue of URLs to be crawled, ensuring:
- No duplicate URLs (via normalized seen set)
- At most one thread per host at any time
- Fair distribution across domains (round-robin)
- Per-host delay enforcement
"""

import collections
import threading
import time
from urllib.parse import urlparse

from url_normalize import url_normalize


_NON_HTML_EXTENSIONS = frozenset({
    ".pdf", ".jpg", ".jpeg", ".png", ".gif", ".svg", ".bmp", ".webp",
    ".zip", ".gz", ".tar", ".rar", ".7z",
    ".mp3", ".mp4", ".avi", ".mov", ".wmv", ".flv", ".webm",
    ".css", ".js", ".json", ".xml",
    ".ico", ".woff", ".woff2", ".ttf", ".eot",
    ".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx",
    ".exe", ".dmg", ".apk",
})


class Frontier:
    """Thread-safe, domain-aware URL frontier with round-robin scheduling."""

    def __init__(self, default_delay: float = 0.1):
        self._seen: set[str] = set()
        self._host_queues: dict[str, collections.deque[str]] = {}
        self._hosts: list[str] = []
        self._host_index: int = 0

        self._host_last_access: dict[str, float] = {}
        self._host_in_use: dict[str, bool] = {}
        self._host_delays: dict[str, float] = {}

        self._default_delay = default_delay
        self._lock = threading.Lock()

    # --- Public API ---

    def seed(self, urls: list[str]) -> None:
        """Initialize the frontier with seed URLs."""
        for url in urls:
            self.add(url)

    def add(self, url: str) -> bool:
        """Normalize and enqueue a URL. Returns False if duplicate or invalid."""
        normalized = self._normalize(url)
        if normalized is None:
            return False

        with self._lock:
            if normalized in self._seen:
                return False

            self._seen.add(normalized)
            host = urlparse(normalized).hostname or ""

            if host not in self._host_queues:
                self._host_queues[host] = collections.deque()
                self._hosts.append(host)
                self._host_in_use[host] = False

            self._host_queues[host].append(normalized)
            return True

    def get_next(self, timeout: float = 1.0) -> tuple[str, str] | None:
        """
        Return the next (url, host) pair respecting round-robin and politeness.

        Blocks up to `timeout` seconds. Returns None if nothing is available.
        """
        deadline = time.monotonic() + timeout

        while time.monotonic() < deadline:
            result = self._try_dequeue()
            if result is not None:
                return result
            time.sleep(0.05)

        return None

    def release_host(self, host: str) -> None:
        """Mark a host as free and update its last-access timestamp."""
        with self._lock:
            self._host_in_use[host] = False
            self._host_last_access[host] = time.monotonic()

            # Remove host from rotation if its queue is empty
            if host in self._host_queues and len(self._host_queues[host]) == 0:
                del self._host_queues[host]
                if host in self._hosts:
                    self._hosts.remove(host)
                    # Adjust index so we don't skip a host
                    if self._host_index >= len(self._hosts) and self._hosts:
                        self._host_index = 0

    def set_host_delay(self, host: str, delay: float) -> None:
        """Set a custom delay for a host (from robots.txt Crawl-delay).

        If robots.txt explicitly specifies a delay, honor it verbatim — the
        spec's 100ms floor applies only "unless explicitly stated otherwise".
        """
        with self._lock:
            self._host_delays[host] = delay

    def is_empty(self) -> bool:
        """Check if there are any URLs left to crawl."""
        with self._lock:
            return len(self._hosts) == 0

    @property
    def seen_count(self) -> int:
        """Number of unique URLs that have been added to the frontier."""
        with self._lock:
            return len(self._seen)

    # --- Internal helpers ---

    def _try_dequeue(self) -> tuple[str, str] | None:
        """Attempt one round-robin pass. Returns (url, host) or None."""
        with self._lock:
            if not self._hosts:
                return None

            n = len(self._hosts)
            for _ in range(n):
                idx = self._host_index % n
                host = self._hosts[idx]
                self._host_index = (idx + 1) % n

                if self._host_in_use.get(host, False):
                    continue

                delay = self._host_delays.get(host, self._default_delay)
                last = self._host_last_access.get(host, 0.0)
                if (time.monotonic() - last) < delay:
                    continue

                queue = self._host_queues.get(host)
                if not queue:
                    continue

                url = queue.popleft()
                self._host_in_use[host] = True
                return (url, host)

        return None

    @staticmethod
    def _normalize(url: str) -> str | None:
        """Normalize a URL, returning None if invalid or non-HTTP(S)."""
        try:
            normalized = url_normalize(url)
        except Exception:
            return None

        # Remove fragment
        frag_pos = normalized.find("#")
        if frag_pos != -1:
            normalized = normalized[:frag_pos]

        # Only keep http and https
        parsed = urlparse(normalized)
        if parsed.scheme not in ("http", "https"):
            return None

        if not parsed.hostname:
            return None

        # Skip URLs with non-HTML file extensions
        path_lower = parsed.path.lower()
        for ext in _NON_HTML_EXTENSIONS:
            if path_lower.endswith(ext):
                return None

        return normalized