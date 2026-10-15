# TP 1 - Web Crawler

**TP 1: Web crawler**  
Course: [RI - Recuperação de Informação](../README.md) (Information Retrieval), 2026/1

Multithreaded web crawler. It keeps a frontier of URLs with round-robin between hosts, respects robots.txt and crawl delays, parses the HTML to find new links and stores the pages in compressed WARC files.

## Files

- [`Documentation.pdf`](Documentation.pdf): Report (English).
- [`code/`](code): Crawler code: `crawler.py` (entry point), `frontier.py`, `fetcher.py`, `parser.py`, `storage.py`, `stats.py`.
- [`link_corpus.txt`](link_corpus.txt): Link to the crawled corpus.

## How to run

```bash
cd code
pip install -r requirements.txt
python3 crawler.py -s seeds-2020007112.txt -n 1000
```

Comments in the code and reports are in Portuguese unless noted otherwise.
