"""Utilidades compartilhadas: paths, leitura de dados, tokenização, escrita de runs.

IDs (QueryId, EntityId) são SEMPRE strings — preservam zeros à esquerda.
Tokenização (alinhada entre indexação e consulta): lowercase, fold de acentos,
[a-z0-9]+, remoção de stopwords, stemming Snowball (PyStemmer).
"""
from __future__ import annotations

import csv
import re
import unicodedata
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "data"
CACHE = ROOT / "cache"
RUNS = ROOT / "runs"
CACHE.mkdir(exist_ok=True)
RUNS.mkdir(exist_ok=True)

CORPUS = DATA / "corpus.jsonl"
TRAIN_QUERIES = DATA / "train_queries.csv"
TEST_QUERIES = DATA / "test_queries.csv"
TRAIN_QRELS = DATA / "train_qrels.csv"

FIELDS = ("title", "keywords", "text")

# Lucene EnglishAnalyzer + interrogativas/auxiliares comuns em queries NL
STOPWORDS = frozenset(
    """a an and are as at be but by for if in into is it no not of on or such
    that the their then there these they this to was will with from which who
    whom whose what where when why how does do did done has have had were been
    being am""".split()
)

_WORD = re.compile(r"[a-z0-9]+")
_stemmer = None  # PyStemmer não é picklável: instância lazy por processo


def _stem():
    global _stemmer
    if _stemmer is None:
        import Stemmer

        _stemmer = Stemmer.Stemmer("english")
    return _stemmer


def _fold(s: str) -> str:
    if s.isascii():
        return s
    return unicodedata.normalize("NFKD", s).encode("ascii", "ignore").decode()


def tokenize(text: str, stop: bool = True) -> list[str]:
    toks = _WORD.findall(_fold(text.lower()))
    if stop:
        toks = [t for t in toks if t not in STOPWORDS]
    return _stem().stemWords(toks)


def load_queries(path: Path) -> list[tuple[str, str]]:
    out = []
    with open(path, newline="", encoding="utf-8") as f:
        for row in csv.DictReader(f):
            out.append((row["QueryId"], row["Query"]))
    return out


def all_queries() -> list[tuple[str, str]]:
    """Train + test, na ordem (train primeiro). qids únicos entre os dois."""
    return load_queries(TRAIN_QUERIES) + load_queries(TEST_QUERIES)


def load_qrels() -> dict[str, dict[str, int]]:
    qrels: dict[str, dict[str, int]] = {}
    with open(TRAIN_QRELS, newline="", encoding="utf-8") as f:
        for row in csv.DictReader(f):
            qrels.setdefault(row["QueryId"], {})[row["EntityId"]] = int(row["Relevance"])
    return qrels


def query_tokens(text: str) -> list[str]:
    """Tokens da query; se a remoção de stopwords zerar, mantém sem remoção."""
    return tokenize(text) or tokenize(text, stop=False)


def write_run(run: dict[str, list[str]], path: Path | str, k: int = 100) -> None:
    """run: qid -> docids em ordem decrescente de relevância. Grava QueryId,EntityId."""
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["QueryId", "EntityId"])
        for qid in sorted(run):
            for did in run[qid][:k]:
                w.writerow([qid, did])


def read_run(path: Path | str) -> dict[str, list[str]]:
    run: dict[str, list[str]] = {}
    with open(path, newline="", encoding="utf-8") as f:
        for row in csv.DictReader(f):
            run.setdefault(row["QueryId"], []).append(row["EntityId"])
    return run
