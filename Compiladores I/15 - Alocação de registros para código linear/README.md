# 15 - Alocação de registros para código linear

**Lab 15: Register allocation**  
Course: [Compiladores I](../README.md) (Compilers I), 2024/2

Register allocation for straight-line code: maps variables to a limited number of registers and spills to memory when needed (LRU policy).

## Files

- [`Asm.py`](Asm.py): Instruction set and interpreter.
- [`Expression.py`](Expression.py)
- [`Lexer.py`](Lexer.py)
- [`Optimizer.py`](Optimizer.py): Register allocator (my implementation).
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
