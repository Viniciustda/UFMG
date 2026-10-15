"""CLI do indexador: python3 indexer.py -m <MEMORY> -c <CORPUS> -i <INDEX>."""

from __future__ import annotations

import argparse
import sys

from tp2.indexer import run


def main() -> int:
    ap = argparse.ArgumentParser(description="Indexador do TP2.")
    ap.add_argument("-m", "--memory", type=int, required=True,
                    help="Memória disponível ao indexador, em MB.")
    ap.add_argument("-c", "--corpus", required=True,
                    help="Caminho para o JSONL do corpus.")
    ap.add_argument("-i", "--index", required=True,
                    help="Diretório de saída do índice.")
    args = ap.parse_args()
    return run(memory_mb=args.memory, corpus_path=args.corpus, index_dir=args.index)


if __name__ == "__main__":
    sys.exit(main())
