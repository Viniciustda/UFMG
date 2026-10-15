# 11 - Código para expressões aritméticas

**Lab 11: Code generation for arithmetic**  
Course: [Compiladores I](../README.md) (Compilers I), 2024/2

Generates low-level (RISC-V like) instructions for arithmetic expressions and runs them in an interpreter.

## Files

- [`Asm.py`](Asm.py): Instruction set and interpreter.
- [`Expression.py`](Expression.py)
- [`Lexer.py`](Lexer.py)
- [`Parser.py`](Parser.py)
- [`Visitor.py`](Visitor.py): Code generator.
- [`driver.py`](driver.py): Test driver given by the instructors.

## How to run

```bash
python3 driver.py < input.txt
```

## Notes

- `driver.py` was given by the instructors and runs the tests of the VPL (Virtual Programming Lab). It reads the program from standard input.

Comments in the code and reports are in Portuguese unless noted otherwise.
