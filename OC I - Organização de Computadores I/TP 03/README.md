# TP 03

**TP 3: Cache simulator**  
Course: [OC I - Organização de Computadores I](../README.md) (Computer Organization I), 2023/2

Group project: simulator of a single-level cache memory. It reads a list of memory addresses and counts hits and misses.

## Files

- [`TP3DCC006-2023-2.docx.pdf`](TP3DCC006-2023-2.docx.pdf): Assignment statement (Portuguese).
- [`entrada1.txt`](entrada1.txt): Example inputs (`entrada1.txt` to `entrada7.txt`).
- [`entrada2.txt`](entrada2.txt)
- [`entrada3.txt`](entrada3.txt)
- [`entrada4.txt`](entrada4.txt)
- [`entrada5.txt`](entrada5.txt)
- [`entrada6.txt`](entrada6.txt)
- [`entrada7.txt`](entrada7.txt)
- [`grupo.txt`](grupo.txt)
- [`output.txt`](output.txt): Example output.
- [`tp.cpp`](tp.cpp): Simulator.

## How to run

```bash
g++ -O2 -o cache tp.cpp
./cache <cache_size> <line_size> <group_size> entrada1.txt
```

Comments in the code and reports are in Portuguese unless noted otherwise.
