"""
Storage module — WARC file writer.

Stores crawled pages in gzip-compressed WARC files:
- 1,000 pages per file
- Files named warc_000.warc.gz through warc_099.warc.gz
- Thread-safe writes via lock
"""

import os
import threading
from io import BytesIO

import requests
from warcio.statusandheaders import StatusAndHeaders
from warcio.warcwriter import WARCWriter


_PAGES_PER_FILE = 1000


class Storage:
    """Thread-safe WARC storage that rotates files every 1,000 records."""

    def __init__(self, output_dir: str = "output"):
        self._output_dir = output_dir
        os.makedirs(output_dir, exist_ok=True)

        self._file_index = 0
        self._page_count = 0
        self._file = None
        self._writer: WARCWriter | None = None
        self._lock = threading.Lock()

        self._open_next_file()

    def store(self, url: str, response: requests.Response) -> None:
        """Write a crawled page as a WARC response record."""
        with self._lock:
            # Lazily rotate: only open the next file right before writing to it, so a crawl that stops on a file boundary doesn't leave an empty trailing .warc.gz behind.
            if self._page_count >= _PAGES_PER_FILE:
                self._close_current_file()
                self._file_index += 1
                self._open_next_file()

            # Build HTTP status line
            status_line = f"{response.status_code} {response.reason}"
            headers_list = list(response.headers.items())
            http_headers = StatusAndHeaders(
                status_line, headers_list, protocol="HTTP/1.1"
            )

            # Create and write the WARC record
            record = self._writer.create_warc_record(
                url,
                "response",
                payload=BytesIO(response.content),
                http_headers=http_headers,
            )
            self._writer.write_record(record)

            self._page_count += 1

    def close(self) -> None:
        """Close the current WARC file."""
        with self._lock:
            self._close_current_file()

    def _open_next_file(self) -> None:
        """Open a new WARC file for writing."""
        filename = f"warc_{self._file_index:03d}.warc.gz"
        path = os.path.join(self._output_dir, filename)
        self._file = open(path, "wb")
        self._writer = WARCWriter(self._file, gzip=True)
        self._page_count = 0

    def _close_current_file(self) -> None:
        """Flush and close the current file handle."""
        if self._file and not self._file.closed:
            self._file.close()
        self._writer = None
        self._file = None