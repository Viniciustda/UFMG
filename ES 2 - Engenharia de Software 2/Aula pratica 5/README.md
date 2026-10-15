# Aula pratica 5

**Practical class 5: Continuous integration**  
Course: [ES 2 - Engenharia de Software 2](../README.md) (Software Engineering II), 2025/1

A simple calculator with pytest tests and a GitHub Actions workflow that installs the dependencies and runs the tests on every push.

## Files

- [`.github/`](.github): GitHub Actions workflow (`build-e-testes.yml`).
- [`calculadora.py`](calculadora.py): Calculator.
- [`requirements.txt`](requirements.txt): Dependencies.
- [`test_calculadora.py`](test_calculadora.py): pytest tests.

## How to run

```bash
pip install -r requirements.txt
pytest
```

## Notes

- GitHub only runs workflows placed at the root of a repository, so this workflow is kept here as reference.

Comments in the code and reports are in Portuguese unless noted otherwise.
