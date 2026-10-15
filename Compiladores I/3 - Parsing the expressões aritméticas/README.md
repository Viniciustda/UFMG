# 3 - Parsing the expressões aritméticas

**Lab 3: Parsing arithmetic expressions**  
Course: [Compiladores I](../README.md) (Compilers I), 2024/2

Recursive descent parser that builds the expression tree respecting operator precedence.

## Files

- [`Expression.py`](Expression.py): Expression classes.
- [`Lexer.py`](Lexer.py): Lexer.
- [`Parser.py`](Parser.py): Parser (my implementation).
- [`driver.py`](driver.py): Test driver given by the instructors.

## How to run

```bash
python3 driver.py < input.txt
```

## Notes

- `driver.py` was given by the instructors and runs the tests of the VPL (Virtual Programming Lab). It reads the program from standard input.

Comments in the code and reports are in Portuguese unless noted otherwise.
