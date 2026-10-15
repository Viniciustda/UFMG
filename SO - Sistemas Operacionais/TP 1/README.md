# TP 1

**TP 1: Basic shell**  
Course: [SO - Sistemas Operacionais](../README.md) (Operating Systems), 2024/2

A simple Unix shell in C. It runs commands with `fork` and `exec`, and supports output and input redirection (`>` and `<`) and pipes (`|`).

## Files

- [`TP01.pdf`](TP01.pdf): Assignment statement (Portuguese).
- [`arquivos/`](arquivos): Shell code (`sh.c`), autograding script, tests and report (`report.txt`).

## How to run

```bash
cd arquivos/arquivos
gcc -Wall -o myshell sh.c
./myshell
./grade.sh   # autograding script from the course
```

Comments in the code and reports are in Portuguese unless noted otherwise.
