"""Leitor streaming do corpus JSONL."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Iterable, Iterator


# Nome do campo de texto pode variar entre dumps do Kaggle.
_TEXT_FIELD_CANDIDATES: tuple[str, ...] = (
    "text",
    "descriptive_text",
    "descriptiveText",
    "description",
    "abstract",
    "body",
)


class CorpusFieldError(RuntimeError):
    pass


def _detect_text_field(path: Path) -> str:
    with open(path, "rb") as fh:
        first = fh.readline()
    if not first:
        raise CorpusFieldError(f"corpus vazio: {path}")
    try:
        obj = json.loads(first.decode("utf-8"))
    except (json.JSONDecodeError, UnicodeDecodeError) as exc:
        raise CorpusFieldError(f"primeira linha malformada: {exc}") from exc
    for cand in _TEXT_FIELD_CANDIDATES:
        if cand in obj:
            return cand
    raise CorpusFieldError(
        f"nenhum dos campos {_TEXT_FIELD_CANDIDATES} encontrado em {sorted(obj)}"
    )


def iter_documents(corpus_path: str | Path) -> Iterator[dict]:
    """Itera o JSONL pulando linhas vazias/malformadas com warning."""
    path = Path(corpus_path)
    text_field = _detect_text_field(path)
    print(f"[reader] campo de texto = '{text_field}'", file=sys.stderr, flush=True)

    skipped = 0
    with open(path, "rb") as fh:
        for raw in fh:
            line = raw.strip()
            if not line:
                continue
            try:
                obj = json.loads(line.decode("utf-8"))
            except (json.JSONDecodeError, UnicodeDecodeError) as exc:
                skipped += 1
                if skipped <= 5:
                    print(f"[reader] linha malformada (#{skipped}): {exc}",
                          file=sys.stderr, flush=True)
                continue
            if text_field != "text":
                obj["text"] = obj.pop(text_field, "")
            yield obj
    if skipped:
        print(f"[reader] total de linhas puladas: {skipped}",
              file=sys.stderr, flush=True)


def batched(it: Iterable[dict], batch_size: int) -> Iterator[list[dict]]:
    batch: list[dict] = []
    for item in it:
        batch.append(item)
        if len(batch) >= batch_size:
            yield batch
            batch = []
    if batch:
        yield batch
