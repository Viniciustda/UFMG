"""CLI do processador: python3 processor.py -i <INDEX> -q <QUERIES> -r <RANKER>."""

from __future__ import annotations

import argparse
import sys

from tp2.processor import run


def main() -> int:
    ap = argparse.ArgumentParser(description="Processador de consultas do TP2.")
    ap.add_argument("-i", "--index", required=True,
                    help="Diretório do índice.")
    ap.add_argument("-q", "--queries", required=True,
                    help="Arquivo com as consultas (uma por linha).")
    ap.add_argument("-r", "--ranker", required=True, choices=["TFIDF", "BM25"],
                    help="Função de ranqueamento.")
    args = ap.parse_args()
    return run(index_dir=args.index, queries_path=args.queries, ranker_name=args.ranker)


if __name__ == "__main__":
    sys.exit(main())
