# TP 2

**TP 2: Software testing**  
Course: [ES - Engenharia de Software](../README.md) (Software Engineering I), 2024/2

Five testing exercises from the book Engenharia de Software Moderna. The starter code comes from the course material and I wrote or fixed the tests.

## Files

- [`Exercício 1 - Teste de Unidade - Carrinho de Compra/`](Exerc%C3%ADcio%201%20-%20Teste%20de%20Unidade%20-%20Carrinho%20de%20Compra): Unit tests with JUnit for a shopping cart.
- [`Exercício 2 - Teste de Unidade - Bugs no Sistema de Bibliotecas/`](Exerc%C3%ADcio%202%20-%20Teste%20de%20Unidade%20-%20Bugs%20no%20Sistema%20de%20Bibliotecas): Unit tests with JUnit that find bugs in a library system.
- [`Exercício 3 - Teste de Integração/`](Exerc%C3%ADcio%203%20-%20Teste%20de%20Integra%C3%A7%C3%A3o): Integration tests with Jest for a small Q&A forum (esmforum).
- [`Exercício 4 - Teste E2E/`](Exerc%C3%ADcio%204%20-%20Teste%20E2E): End-to-end tests with Cypress for a to-do app.
- [`Exercício 5 - Integração Contínua/`](Exerc%C3%ADcio%205%20-%20Integra%C3%A7%C3%A3o%20Cont%C3%ADnua): Continuous integration with Maven and GitHub Actions.

## How to run

```bash
# Exercises 1 and 2 (Java + JUnit 4):
cd "Exercício 1 - Teste de Unidade - Carrinho de Compra"
javac -cp "lib/*" -d class *.java
java -cp "class:lib/*" TestRunner

# Exercise 3 (Jest):
cd "Exercício 3 - Teste de Integração/esmforum" && npm install && npm test
```

Comments in the code and reports are in Portuguese unless noted otherwise.
