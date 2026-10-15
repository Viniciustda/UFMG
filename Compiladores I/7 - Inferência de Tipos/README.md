# 7 - Inferência de Tipos

**Lab 7: Type inference**  
Course: [Compiladores I](../README.md) (Compilers I), 2024/2

Infers the types of variables by generating constraints and solving them with unification.

## Files

- [`Expression.py`](Expression.py)
- [`Lexer.py`](Lexer.py)
- [`Parser.py`](Parser.py)
- [`Unifier.py`](Unifier.py): Unification algorithm.
- [`Visitor.py`](Visitor.py): Constraint generation.
- [`driver.py`](driver.py): Test driver given by the instructors.

## How to run

```bash
python3 driver.py < input.txt
```

## Notes

- `driver.py` was given by the instructors and runs the tests of the VPL (Virtual Programming Lab). It reads the program from standard input.

Comments in the code and reports are in Portuguese unless noted otherwise.
