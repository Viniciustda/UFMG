# Versões

**Spiral algorithms with different complexities**  
Course: [MD - Matemática Discreta](../README.md) (Discrete Mathematics), 2020/2 and 2021/1

Alternative versions of the square and triangular spiral programs (see the course README). Each version finds the (x, y) coordinates of point n with a different time complexity.

## Files

- [`O(1)/`](O%281%29): Constant time versions (closed formula).
- [`O(n)/`](O%28n%29): Linear time versions (walk the spiral).
- [`menor que O(n)/`](menor%20que%20O%28n%29): Sub-linear versions.

## How to run

```bash
gcc -o quadrada "O(1)/espquadrada - O(1).c" -lm
echo 10 | ./quadrada
```

Comments in the code and reports are in Portuguese unless noted otherwise.
