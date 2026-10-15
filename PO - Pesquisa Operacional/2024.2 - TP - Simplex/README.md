# 2024.2 - TP - Simplex

**Simplex method (2024/2)**  
Course: [PO - Pesquisa Operacional](../README.md) (Operations Research), 2024/2

Solves general linear programs with the tableau Simplex method. The program reads a linear program from a text file, converts it to standard form, prints every pivot step and reports whether the problem is optimal, infeasible or unbounded, with the objective value, the primal solution and the dual solution.

## Files

- [`TP_20242.pdf`](TP_20242.pdf): Assignment statement (Portuguese). Describes the input and output formats.
- [`exemplos/`](exemplos): Example linear programs.
- [`out/`](out): Expected outputs for the examples.
- [`simplex.py`](simplex.py): Simplex implementation.

## How to run

```bash
pip install numpy
python3 simplex.py exemplos/01
python3 simplex.py exemplos/01 --decimals 3 --digits 7
```

Comments in the code and reports are in Portuguese unless noted otherwise.
