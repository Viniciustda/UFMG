# TP 2

**TP 2: Virtual memory simulator**  
Course: [SO - Sistemas Operacionais](../README.md) (Operating Systems), 2024/2

Done in pairs. Simulates a virtual memory system: reads a trace of memory accesses and reports the number of page faults, reads and writes. It supports the LRU, FIFO, random and second chance replacement policies and four page table types (dense, two-level, three-level and inverted).

## Files

- [`Makefile`](Makefile): Build.
- [`algoritmos.c`](algoritmos.c): Replacement algorithms.
- [`algoritmos.h`](algoritmos.h)
- [`auxiliares.c`](auxiliares.c): Helpers.
- [`auxiliares.h`](auxiliares.h)
- [`inicializacao.c`](inicializacao.c): Initialization.
- [`inicializacao.h`](inicializacao.h)
- [`listas.c`](listas.c): Lists used by the algorithms.
- [`listas.h`](listas.h)
- [`relatorio.pdf`](relatorio.pdf): Report (Portuguese).
- [`simulacao.c`](simulacao.c): Simulation loop.
- [`simulacao.h`](simulacao.h)
- [`structs.h`](structs.h)
- [`tp2.pdf`](tp2.pdf): Assignment statement (Portuguese).
- [`tp2virtual.c`](tp2virtual.c): Main program.

## How to run

```bash
make
./tp2virtual.exe <lru|fifo|random|2a> <trace_file> <page_size_KB> <memory_size_KB> [densa|hierarquica2|hierarquica3|inversa]
```

Comments in the code and reports are in Portuguese unless noted otherwise.
