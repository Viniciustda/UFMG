"""Pré-processamento de texto compartilhado por indexer e processor.

Pipeline: NFKD-strip-ASCII → lowercase → regex [a-z0-9]{3,} → remove
stopwords (NLTK English) → Porter stemming memoizado.
"""

from __future__ import annotations

import re
import unicodedata
from functools import lru_cache
from pathlib import Path
from typing import Iterator

import nltk

from tp2.config import MIN_TOKEN_LEN, STEM_CACHE_SIZE


# Carrega o nltk_data empacotado se existir (offline-safe).
_HERE = Path(__file__).resolve().parent
_BUNDLED_NLTK_DATA = (_HERE.parent / "nltk_data").resolve()
if _BUNDLED_NLTK_DATA.is_dir():
    nltk.data.path.insert(0, str(_BUNDLED_NLTK_DATA))


_TOKEN_RE = re.compile(rf"[a-z0-9]{{{MIN_TOKEN_LEN},}}")

_STOPWORDS: frozenset[str] | None = None
_STEMMER = None


def _load_stopwords() -> frozenset[str]:
    from nltk.corpus import stopwords
    try:
        words = stopwords.words("english")
    except LookupError:
        nltk.download("stopwords", quiet=True)
        words = stopwords.words("english")
    return frozenset(words)


def _get_stopwords() -> frozenset[str]:
    global _STOPWORDS
    if _STOPWORDS is None:
        _STOPWORDS = _load_stopwords()
    return _STOPWORDS


def _get_stemmer():
    global _STEMMER
    if _STEMMER is None:
        from nltk.stem import PorterStemmer
        _STEMMER = PorterStemmer()
    return _STEMMER


@lru_cache(maxsize=STEM_CACHE_SIZE)
def _stem(token: str) -> str:
    return _get_stemmer().stem(token)


def _normalize(text: str) -> str:
    nfkd = unicodedata.normalize("NFKD", text)
    return nfkd.encode("ascii", "ignore").decode("ascii").lower()


def analyze(text: str) -> Iterator[str]:
    """Yield stems de um trecho de texto."""
    if not text:
        return
    norm = _normalize(text)
    stops = _get_stopwords()
    for match in _TOKEN_RE.finditer(norm):
        tok = match.group()
        if tok in stops:
            continue
        # Stopwords filtradas antes do stem pra evitar "wa", "ha" etc.
        yield _stem(tok)


def analyze_to_list(text: str) -> list[str]:
    return list(analyze(text))


def warm_up() -> None:
    """Força carga eager dentro de um worker."""
    _get_stopwords()
    _get_stemmer()
