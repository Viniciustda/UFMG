# tp2

**TP 2: Desemprego**  
Course: [Alg I - Algoritmos I](../README.md) (Algorithms I), 2023/1

Matches users of a job network to job openings (bipartite graph). A greedy algorithm gives a fast answer, and an exact algorithm based on augmenting paths (DFS) finds the best possible matching.

## Files

- [`Documentacao.pdf`](Documentacao.pdf): Report (Portuguese).
- [`Makefile`](Makefile): Build and test targets.
- [`main.cpp`](main.cpp): Greedy and exact algorithms.
- [`test_cases/`](test_cases): Test cases: inputs and expected outputs (`.sol`).

## How to run

```bash
make
./tp02 < test_cases/inputs/test_case1.txt
make eval
```

Comments in the code and reports are in Portuguese unless noted otherwise.
