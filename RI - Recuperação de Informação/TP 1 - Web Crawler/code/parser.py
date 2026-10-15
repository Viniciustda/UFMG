"""
Parser module — HTML content extraction.

Extracts from HTML documents:
- Page title
- Visible text (first 20 words for debug mode)
- Outgoing links (resolved to absolute URLs)
"""

from urllib.parse import urljoin, urlparse

from bs4 import BeautifulSoup


# Schemes to follow
_ALLOWED_SCHEMES = frozenset({"http", "https"})

# href prefixes to ignore
_IGNORED_PREFIXES = ("mailto:", "javascript:", "tel:", "data:", "ftp:")

# File extensions unlikely to be HTML
_NON_HTML_EXTENSIONS = frozenset({
    ".pdf", ".jpg", ".jpeg", ".png", ".gif", ".svg", ".bmp", ".webp",
    ".zip", ".gz", ".tar", ".rar", ".7z",
    ".mp3", ".mp4", ".avi", ".mov", ".wmv", ".flv", ".webm",
    ".css", ".js", ".json", ".xml",
    ".ico", ".woff", ".woff2", ".ttf", ".eot",
    ".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx",
    ".exe", ".dmg", ".apk",
})


def parse(html: str, base_url: str) -> tuple[list[str], str, str, int]:
    """
    Parse an HTML document and extract links, title, and visible text.

    Args:
        html: Raw HTML string.
        base_url: The URL of the page (used to resolve relative links).

    Returns:
        (links, title, first_20_words, num_visible_tokens) where:
        - links: list of absolute URLs found in <a href> tags
        - title: page title (empty string if absent)
        - first_20_words: first 20 whitespace-separated words of visible text
        - num_visible_tokens: total word count of visible text
    """
    soup = BeautifulSoup(html, "html.parser")

    # --- Title ---
    title = ""
    if soup.title:
        title = soup.title.get_text(strip=True)

    # --- Visible text (first 20 words) ---
    # Strip non-visible tags before extracting text so script/style/etc
    # don't leak into the token count or debug preview.
    for tag in soup(["script", "style", "noscript", "template"]):
        tag.decompose()

    raw_text = soup.get_text(separator=" ", strip=True)
    words = raw_text.split()
    first_20 = " ".join(words[:20])

    # --- Links ---
    links: list[str] = []
    for tag in soup.find_all("a", href=True):
        href = tag["href"].strip()

        if not href or any(href.lower().startswith(p) for p in _IGNORED_PREFIXES):
            continue

        absolute = urljoin(base_url, href)
        parsed = urlparse(absolute)

        if parsed.scheme not in _ALLOWED_SCHEMES:
            continue

        if not parsed.hostname:
            continue

        # Skip non-HTML file extensions
        path_lower = parsed.path.lower()
        if any(path_lower.endswith(ext) for ext in _NON_HTML_EXTENSIONS):
            continue

        links.append(absolute)

    return (links, title, first_20, len(words))