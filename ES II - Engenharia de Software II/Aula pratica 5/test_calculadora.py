# test_calculadora.py (versao para pytest)
import pytest
from calculadora import somar, subtrair, multiplicar, dividir, potencia, porcentagem

def test_somar():
    assert somar(5, 3) == 8

def test_subtrair():
    assert subtrair(10, 4) == 6

def test_multiplicar():
    assert multiplicar(2, 5) == 10

def test_dividir():
    assert dividir(10, 2) == 5

def test_dividir_por_zero():
    with pytest.raises(ValueError, match="Nao e possivel dividir por zero"):
        dividir(10, 0)

def test_potencia():
    assert potencia(2, 3) == 8

def test_porcentagem():
    assert porcentagem(66, 50) == 33