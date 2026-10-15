# tp1

**TP 1: Aritmofobia**  
Course: [Alg I - Algoritmos I](../README.md) (Algorithms I), 2023/1

Finds the shortest path between two cities when only roads with even length can be used and the path must have an even number of roads. The graph is doubled into "even" and "odd" copies of each vertex and Dijkstra's algorithm runs on it.

## Files

- [`Documentacao.pdf`](Documentacao.pdf): Report (Portuguese).
- [`Makefile`](Makefile): Build and test targets.
- [`graph.cpp`](graph.cpp): Graph and shortest path implementation.
- [`graph.hpp`](graph.hpp): Graph interface.
- [`main.cpp`](main.cpp): Reads the input and prints the answer.
- [`test_cases/`](test_cases): Test cases: inputs and expected outputs (`.sol`).

## How to run

```bash
make
./tp01 < test_cases/inputs/test_case1.txt
make eval   # runs all test cases
```

Comments in the code and reports are in Portuguese unless noted otherwise.
