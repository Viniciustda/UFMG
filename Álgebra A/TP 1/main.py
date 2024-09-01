import sys
import time
from prime import findNextPrime, printPrime
from factorization import factorazation
from generator import findGenerator, printGenerator
from discreteLogarithm import logDiscrete
import utils

def main(filename):
    tempo_inicial = time.time()

    with open(filename, "r") as f:
        N = int(f.readline())
        a = int(f.readline())

    # Encontrar o menor primo p > N
    p, qt = findNextPrime(N)
    printPrime(p, qt)

    # Fatorar p-1
    fatorizacao, resto = factorazation(p - 1)

    # Encontrar um gerador g
    g, ordemMinima = findGenerator(p, fatorizacao, resto)
    printGenerator(g, ordemMinima, resto)
    
    a = a % p
    log = logDiscrete(g,a,p,fatorizacao,ordemMinima)

    if log is None:
        print('Não foi possível calcular o logaritmo discreto em tempo razoável')
    else:
        print('Logaritmo discreto:',log)
        tempo_final = time.time() - tempo_inicial
        print('Tempo de execução em segundos:', tempo_final)

    with open('resultado.txt', 'w') as f:
        f.write(f'Primo: {p}\n')
        f.write(f'Número de testes Miller-Rabin: {qt}\n')
        if resto == 1:
            f.write(f'Gerador: {g}\n')
        else:
            f.write(f'Elemento com ordem alta: {g}\n')
            f.write(f'Ordem mínima: {ordemMinima}\n')
        if log is not None:
            f.write(f'Logaritmo discreto: {log}\n')
            f.write(f'Tempo de execução: {tempo_final}\n')
        else:
            f.write(f'Não foi possível calcular o logaritmo discreto em tempo razoável')

if __name__ == "__main__":
    if len(sys.argv) > 1:
        main(sys.argv[1])
    else:
        print("É necessário inserir o nome do arquivo na linha de comando")
