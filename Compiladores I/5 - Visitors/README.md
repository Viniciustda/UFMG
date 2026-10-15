# 5 - Visitors

**Lab 5: Visitors**  
Course: [Compiladores I](../README.md) (Compilers I), 2024/2

Moves evaluation out of the expression classes using the Visitor design pattern.

## Files

- [`Expression.py`](Expression.py)
- [`Lexer.py`](Lexer.py)
- [`Parser.py`](Parser.py)
- [`Visitor.py`](Visitor.py): Evaluation visitor (my implementation).
- [`driver.py`](driver.py): Test driver given by the instructors.

## How to run

```bash
python3 driver.py < input.txt
```

## Notes

- `driver.py` was given by the instructors and runs the tests of the VPL (Virtual Programming Lab). It reads the program from standard input.

Comments in the code and reports are in Portuguese unless noted otherwise.
