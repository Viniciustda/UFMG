# TP 1

**TP 1: Discrete logarithm**  
Course: [Álgebra A](../README.md) (Algebra A (number theory)), 2024/1

Group project. Given N and a, the program finds the smallest prime p greater than N (Miller-Rabin test), factors p - 1, finds a generator of the multiplicative group mod p and computes the discrete logarithm of a.

## Files

- [`Relatório TP1 Algebra A.pdf`](Relat%C3%B3rio%20TP1%20Algebra%20A.pdf): Report (Portuguese).
- [`discreteLogarithm.py`](discreteLogarithm.py): Discrete logarithm.
- [`factorization.py`](factorization.py): Factorization of p - 1.
- [`generator.py`](generator.py): Generator search.
- [`main.py`](main.py): Entry point.
- [`prime.py`](prime.py): Next prime with Miller-Rabin.
- [`utils.py`](utils.py): Helpers.

## How to run

```bash
printf "100\n7\n" > input.txt   # N on the first line, a on the second
python3 main.py input.txt
```

Comments in the code and reports are in Portuguese unless noted otherwise.
