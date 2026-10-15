# Atividade 3 - Vulnerabilidades em C

**Activity 3: Vulnerabilities in a C program**  
Course: [Cibersegurança](../README.md) (Cybersecurity), 2025/2

Analysis of a small C program with two memory vulnerabilities (an integer overflow in a size calculation and a buffer overflow with `strcpy`) and a fixed version.

## Files

- [`Análise de Vulnerabilidades em Programa C.pdf`](An%C3%A1lise%20de%20Vulnerabilidades%20em%20Programa%20C.pdf): Report (Portuguese).
- [`vuln.c`](vuln.c): Vulnerable program (for study only).
- [`vuln_fixed.c`](vuln_fixed.c): Fixed version.

## How to run

```bash
gcc -o vuln vuln.c && ./vuln "text" 3
gcc -o vuln_fixed vuln_fixed.c && ./vuln_fixed "text" 3
```

Comments in the code and reports are in Portuguese unless noted otherwise.
