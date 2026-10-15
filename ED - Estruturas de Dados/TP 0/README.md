# TP 0

**TP 0: Matrix operations**  
Course: [ED - Estruturas de Dados](../README.md) (Data Structures), 2021/2

Matrix abstract data type (sum, product and transpose) with a library that logs every memory access, used to study memory locality.

## Files

- [`tp0viniciustrindade/`](tp0viniciustrindade): Source code (`src/`), headers (`include/`) and Makefile.

## How to run

```bash
cd tp0viniciustrindade
mkdir -p obj bin
make
./bin/matop -s -x 5 -y 5 -p log.out   # -s sum, -m multiply, -t transpose, -l log accesses
```

## Notes

- `matop.c` and `memlog.c` were provided by the instructors. My work is the matrix type in `mat.c`.

Comments in the code and reports are in Portuguese unless noted otherwise.
