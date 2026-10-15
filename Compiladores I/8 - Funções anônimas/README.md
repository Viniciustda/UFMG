# 8 - Funções anônimas

**Lab 8: Anonymous functions**  
Course: [Compiladores I](../README.md) (Compilers I), 2024/2

Adds anonymous functions (`fn x => ...`), function application and closures.

## Files

- [`Expression.py`](Expression.py)
- [`Lexer.py`](Lexer.py)
- [`Parser.py`](Parser.py): Parser with `fn` expressions.
- [`Visitor.py`](Visitor.py): Evaluation with closures.
- [`driver.py`](driver.py): Test driver given by the instructors.

## How to run

```bash
python3 driver.py < input.txt
```

## Notes

- `driver.py` was given by the instructors and runs the tests of the VPL (Virtual Programming Lab). It reads the program from standard input.

Comments in the code and reports are in Portuguese unless noted otherwise.
