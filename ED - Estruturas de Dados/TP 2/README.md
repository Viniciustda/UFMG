# TP 2

**TP 2: External memory sorting**  
Course: [ED - Estruturas de Dados](../README.md) (Data Structures), 2021/2

Sorts a file that does not fit in memory: first creates sorted runs with quicksort, then merges them with a heap.

## Files

- [`Documentacao.pdf`](Documentacao.pdf): Report (Portuguese).
- [`TP/`](TP): Source code (`src/`), headers (`include/`) and Makefile.
- [`tp2-OrdenacaoExterna.pdf`](tp2-OrdenacaoExterna.pdf): Assignment statement (Portuguese).

## How to run

```bash
cd TP
mkdir -p obj bin
make
./bin/tp2 input.txt output.txt <entries_per_run> [number_of_runs]
```

Comments in the code and reports are in Portuguese unless noted otherwise.
