"""
Web Crawler — main entry point and orchestrator.

Usage:
    python3 crawler.py -s <SEEDS> [-n <LIMIT>] [-d] [-t <THREADS>]

Coordinates multiple worker threads that:
1. Pull URLs from the domain-aware frontier
2. Check robots.txt via the fetcher
3. Fetch and parse HTML pages
4. Store raw content in WARC files
5. Collect metrics for the technical report
"""

import argparse
import json
import sys
import threading
import time

from fetcher import Fetcher
from frontier import Frontier
from parser import parse
from stats import Stats
from storage import Storage


_DEFAULT_LIMIT = 100_000
_DEFAULT_THREADS = 70
_PRINT_LOCK = threading.Lock()


def debug_print(url: str, title: str, text_20: str) -> None:
    """Print a debug JSON record to stdout (thread-safe)."""
    record = {
        "URL": url,
        "Title": title,
        "Text": text_20,
        "Timestamp": int(time.time()),
    }
    line = json.dumps(record, ensure_ascii=False)
    with _PRINT_LOCK:
        print(line, flush=True)


def worker(
    frontier: Frontier,
    fetcher: Fetcher,
    storage: Storage,
    stats: Stats,
    limit: int,
    debug: bool,
    stop_event: threading.Event,
) -> None:
    """Crawl loop executed by each worker thread."""
    while not stop_event.is_set():
        # Get next URL from frontier (with round-robin + delay)
        result = frontier.get_next(timeout=2.0)
        if result is None:
            # Check if frontier is permanently empty
            if frontier.is_empty():
                stop_event.set()
            continue

        url, host = result

        try:
            # Check robots.txt (cached)
            allowed, delay = fetcher.is_allowed(url)
            if delay is not None:
                frontier.set_host_delay(host, delay)
            if not allowed:
                frontier.release_host(host)
                continue

            # Fetch the page
            response = fetcher.fetch(url)
            frontier.release_host(host)

            if response is None:
                continue

            # Verify it's HTML
            if not fetcher.is_html(response):
                continue

            # Parse HTML: extract links, title, visible text
            links, title, text_20, num_tokens = parse(response.text, url)

            # Only store if under limit
            if not stats.try_claim(limit):
                stop_event.set()
                continue

            # Store in WARC
            storage.store(url, response)

            # Record metrics
            num_bytes = len(response.content)
            stats.record_page(url, num_tokens, num_bytes)

            # Debug output
            if debug:
                debug_print(url, title, text_20)

            # Add discovered links to frontier
            for link in links:
                frontier.add(link)

            # Check stop condition
            if stats.pages_crawled >= limit:
                stop_event.set()

        except Exception:
            # Release host on unexpected errors to avoid deadlock
            frontier.release_host(host)


def main() -> None:
    # Ensure UTF-8 output 
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    if hasattr(sys.stderr, "reconfigure"):
        sys.stderr.reconfigure(encoding="utf-8", errors="replace")

    arg_parser = argparse.ArgumentParser(
        description="Web crawler for Information Retrieval (UFMG PA1)"
    )
    arg_parser.add_argument(
        "-s", required=True, help="Path to seed URLs file"
    )
    arg_parser.add_argument(
        "-n", type=int, default=_DEFAULT_LIMIT,
        help=f"Target number of pages to crawl (default: {_DEFAULT_LIMIT})"
    )
    arg_parser.add_argument(
        "-d", action="store_true", help="Enable debug mode"
    )
    arg_parser.add_argument(
        "-t", "--threads", type=int, default=_DEFAULT_THREADS,
        help=f"Number of worker threads (default: {_DEFAULT_THREADS})"
    )
    args = arg_parser.parse_args()

    # Read seed URLs
    with open(args.s, "r", encoding="utf-8") as f:
        seeds = [line.strip() for line in f if line.strip()]

    if not seeds:
        print("Error: no seed URLs found in the file.", file=sys.stderr)
        sys.exit(1)

    # Initialize modules
    frontier = Frontier()
    fetcher = Fetcher()
    storage = Storage()
    stats_collector = Stats()
    stop_event = threading.Event()

    # Seed the frontier
    frontier.seed(seeds)

    print(f"Starting crawl: limit={args.n}, threads={args.threads}, "
          f"seeds={len(seeds)}, debug={args.d}", file=sys.stderr)

    # Launch worker threads
    threads: list[threading.Thread] = []
    for i in range(args.threads):
        t = threading.Thread(
            target=worker,
            args=(frontier, fetcher, storage, stats_collector,
                  args.n, args.d, stop_event),
            name=f"worker-{i}",
            daemon=True,
        )
        t.start()
        threads.append(t)

    # Wait for completion
    try:
        while not stop_event.is_set():
            stop_event.wait(timeout=5.0)
            pages = stats_collector.pages_crawled
            print(f"\rProgress: {pages}/{args.n} pages crawled",
                  end="", flush=True, file=sys.stderr)
    except KeyboardInterrupt:
        print("\nInterrupted by user. Stopping...", file=sys.stderr)
        stop_event.set()

    # Give threads a moment to finish
    for t in threads:
        t.join(timeout=5.0)

    # Finalize
    storage.close()
    stats_collector.dump_report("crawl_stats.json")

    pages = stats_collector.pages_crawled
    print(f"\nCrawl finished: {pages} pages collected.", file=sys.stderr)
    print(f"Stats saved to crawl_stats.json", file=sys.stderr)


if __name__ == "__main__":
    main()