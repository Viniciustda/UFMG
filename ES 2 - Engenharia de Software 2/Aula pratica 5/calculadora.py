# calculadora.py
def somar(a, b):
    return a + b

def subtrair(a, b):
    return a - b

def multiplicar(a, b):
    return a * b

def dividir(a, b):
    if b == 0:
        raise ValueError("Nao e possivel dividir por zero")
    return a / b

def potencia(base, expoente):
    return base ** expoente

def porcentagem(numero, percentual):
    return (numero * percentual) / 100