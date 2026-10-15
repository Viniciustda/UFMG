# 6 - Verificação dinâmica de tipos

**Lab 6: Dynamic type checking**  
Course: [Compiladores I](../README.md) (Compilers I), 2024/2

Adds booleans, comparisons, `if-then-else` and checks types while evaluating the program.

## Files

- [`Expression.py`](Expression.py)
- [`Lexer.py`](Lexer.py)
- [`Parser.py`](Parser.py): Parser with the full precedence table.
- [`Visitor.py`](Visitor.py): Evaluation with type checks.
- [`driver.py`](driver.py): Test driver given by the instructors.

## How to run

```bash
python3 driver.py < input.txt
```

## Notes

- `driver.py` was given by the instructors and runs the tests of the VPL (Virtual Programming Lab). It reads the program from standard input.

Comments in the code and reports are in Portuguese unless noted otherwise.
