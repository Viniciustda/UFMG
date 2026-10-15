"""
Fetcher module — HTTP requests and robots.txt enforcement.

Handles all network communication:
- Fetching web pages via HTTP GET
- Caching and querying robots.txt per host
- Extracting Crawl-delay directives
- Content-Type validation
"""

import threading
from urllib.parse import urlparse

import requests
from protego import Protego


_REQUEST_TIMEOUT = 10  # seconds
_ROBOTS_TIMEOUT = 5    # seconds
USER_AGENT = "UFMG-RI-Crawler/1.0"


class Fetcher:
    """Thread-safe HTTP fetcher with robots.txt caching."""

    def __init__(self):
        # Maps host -> (Protego instance or None, crawl_delay or None)
        self._robots_cache: dict[str, tuple[Protego | None, float | None]] = {}
        self._robots_lock = threading.Lock()

    def is_allowed(self, url: str) -> tuple[bool, float | None]:
        """
        Check whether the URL is allowed by robots.txt.

        Returns:
            (allowed, crawl_delay) where crawl_delay is the Crawl-delay
            from robots.txt if present, or None otherwise.
        """
        parsed = urlparse(url)
        host = parsed.hostname or ""
        scheme = parsed.scheme or "https"

        with self._robots_lock:
            cached = self._robots_cache.get(host)

        if cached is None:
            # Fetch and parse robots.txt
            robots_url = f"{scheme}://{host}/robots.txt"
            protego_obj = None
            crawl_delay = None

            try:
                resp = requests.get(
                    robots_url,
                    timeout=_ROBOTS_TIMEOUT,
                    headers={"User-Agent": USER_AGENT},
                )
                if resp.status_code == 200:
                    protego_obj = Protego.parse(resp.text)
                    crawl_delay = protego_obj.crawl_delay(USER_AGENT)
                    if crawl_delay is None:
                        crawl_delay = protego_obj.crawl_delay("*")
            except Exception:
                pass  # Treat unreachable robots.txt as "allow all"

            with self._robots_lock:
                self._robots_cache[host] = (protego_obj, crawl_delay)
            cached = (protego_obj, crawl_delay)

        protego_obj, crawl_delay = cached

        if protego_obj is None:
            return (True, crawl_delay)

        allowed = protego_obj.can_fetch(url, USER_AGENT)
        return (allowed, crawl_delay)

    def fetch(self, url: str) -> requests.Response | None:
        """
        Perform an HTTP GET request.

        Returns the Response object on success, or None on any error.
        """
        try:
            response = requests.get(
                url,
                timeout=_REQUEST_TIMEOUT,
                headers={"User-Agent": USER_AGENT},
                allow_redirects=True,
            )
        except (requests.Timeout,
                requests.ConnectionError,
                requests.TooManyRedirects,
                requests.RequestException):
            return None

        if response.status_code >= 400:
            return None

        return response

    @staticmethod
    def is_html(response: requests.Response) -> bool:
        """Check if the response Content-Type indicates an HTML document."""
        content_type = response.headers.get("Content-Type", "")
        return "text/html" in content_type.lower()