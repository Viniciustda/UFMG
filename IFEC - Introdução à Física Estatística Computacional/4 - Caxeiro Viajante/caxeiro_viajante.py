# -*- coding: utf-8 -*-
"""
# O Problema do Caixeiro Viajante: Solução por Simulated Annealing

Vinicius Trindade Dias Abel - 2020007112

## Implementação
"""

import sys
import numpy as np
import numpy.typing as npt
import scipy
import matplotlib.pyplot as plt
from numba import jit
import time

def ler_cidades(arquivo):
    with open(arquivo, "r") as f:
        linhas = f.readlines()
    
    cidades = []
    for linha in linhas:
        partes = linha.strip().split()
        # Verifica se há pelo menos 3 colunas: ID, X, Y
        if len(partes) < 3:
            continue
        try:
            # Converte as coordenadas para float
            x, y = map(float, partes[1:3])
            cidades.append([x, y])
        except ValueError:
            print(f"Erro ao processar linha: {linha.strip()}")
            continue

    return np.array(cidades)


def gerar_grafo_aleatorio(numero_de_nos: np.int_) -> npt.NDArray[np.float_]:
    nos: npt.NDArray[np.float_] = np.random.rand(numero_de_nos, 2)
    matriz_adjacencia: npt.NDArray[np.float_] = scipy.spatial.distance_matrix(nos, nos)
    return matriz_adjacencia, nos

def plotar_caminho(nos: npt.NDArray[np.float_], caminho: npt.NDArray[np.int_], custo_caminho: np.float_) -> None:
    plt.figure(figsize=(4, 4))
    plt.title(f"Custo do caminho: {custo_caminho}")
    plt.scatter(nos[:, 0], nos[:, 1], c="black")
    for i in range(caminho.shape[0] - 1):
        coordenadas_x: npt.NDArray[np.float_] = np.array([nos[caminho[i]][0], nos[caminho[i + 1]][0]])
        coordenadas_y: npt.NDArray[np.float_] = np.array([nos[caminho[i]][1], nos[caminho[i + 1]][1]])
        plt.plot(coordenadas_x, coordenadas_y, "b-")
    plt.show()

def plotar_distancia_temperatura(distancias: npt.NDArray[np.float_], temperaturas: npt.NDArray[np.float_], numero_de_cidades: np.float_, temperatura_inicial: np.float_, delta_t: np.float_, limite_inferior_temperatura: np.float_) -> None:
    _, ax1 = plt.subplots(figsize=(6, 4))
    ax1.plot(distancias, color="blue")
    ax1.set_ylabel("Distância percorrida no caminho escolhido", color="blue")
    ax1.set_xlabel("Passos de Monte Carlo")

    ax2 = ax1.twinx()
    ax2.plot(temperaturas, color="red")
    ax2.set_ylabel("Temperatura", color="red")

    plt.title(f"Número de cidades: {numero_de_cidades}, Temperatura inicial: {temperatura_inicial}\n Delta T: {delta_t}, Limite inferior de temperatura: {limite_inferior_temperatura}")
    plt.show()

@jit(nopython=True)
def realizar_passos_monte_carlo(numero_de_passos_monte_carlo: np.int_, numero_de_cidades: np.int_, temperatura: np.float_, grafo: npt.NDArray[np.float_], nos: npt.NDArray[np.float_], caminho_atual: npt.NDArray[np.int_], custo_caminho_atual: np.float_) -> np.float_:
    for _ in range(numero_de_passos_monte_carlo):
        for _ in range(numero_de_cidades):
            x_proposto, y_proposto = np.sort(np.random.choice(np.arange(numero_de_cidades), size=2, replace=False))
            caminho_atual = np.roll(caminho_atual, shift=1)
            caminho_proposto: npt.NDArray[np.int_] = caminho_atual.copy()
            caminho_proposto[x_proposto + 1: y_proposto] = np.flip(caminho_proposto[x_proposto + 1: y_proposto])

            custo_remover_aresta_1: np.float_ = grafo[caminho_atual[x_proposto]][caminho_atual[(x_proposto + 1) % numero_de_cidades]]
            custo_remover_aresta_2: np.float_ = grafo[caminho_atual[y_proposto - 1]][caminho_atual[y_proposto]]
            custo_adicionar_aresta_1: np.float_ = grafo[caminho_proposto[x_proposto]][caminho_proposto[(x_proposto + 1) % numero_de_cidades]]
            custo_adicionar_aresta_2: np.float_ = grafo[caminho_proposto[y_proposto - 1]][caminho_proposto[y_proposto]]

            decrescimo: np.float_ = custo_remover_aresta_1 + custo_remover_aresta_2
            aumento: np.float_ = custo_adicionar_aresta_1 + custo_adicionar_aresta_2
            custo_caminho_proposto: np.float_ = custo_caminho_atual - decrescimo + aumento
            delta: np.float_ = custo_caminho_proposto - custo_caminho_atual
            r: np.float_ = np.random.rand()
            P: np.float_ = np.exp(-1 * delta / temperatura)
            if delta < 0 or r <= P:
                caminho_atual = caminho_proposto.copy()
                custo_caminho_atual = custo_caminho_proposto
    return caminho_atual, custo_caminho_atual

def caixeiro_viajante(grafo: npt.NDArray[np.float_], nos: npt.NDArray[np.float_], temperatura: np.float_, delta_t: np.float_, limite_inferior_temperatura: np.float_) -> tuple:
    numero_de_passos_monte_carlo: np.int_ = 1  # hard coded
    numero_de_cidades: np.int_ = grafo.shape[0]
    caminho_atual: npt.NDArray[np.int_] = np.arange(numero_de_cidades)
    np.random.shuffle(caminho_atual)
    arestas: npt.NDArray[np.int_] = np.append(np.lib.stride_tricks.sliding_window_view(caminho_atual, 2), [[caminho_atual[-1], caminho_atual[0]]], axis=0)
    custo_caminho_atual: np.float_ = np.sum([grafo[i, j] for i, j in arestas])

    temperaturas: list[float] = []
    distancias: list[float] = []

    while temperatura > limite_inferior_temperatura:
        temperaturas.append(temperatura)
        distancias.append(custo_caminho_atual)
        caminho_atual, custo_caminho_atual = realizar_passos_monte_carlo(
            numero_de_passos_monte_carlo=numero_de_passos_monte_carlo,
            numero_de_cidades=numero_de_cidades,
            temperatura=temperatura,
            grafo=grafo,
            nos=nos,
            caminho_atual=caminho_atual,
            custo_caminho_atual=custo_caminho_atual
        )
        temperatura *= delta_t

    caminho_atual = np.append(caminho_atual, caminho_atual[0])
    return caminho_atual, custo_caminho_atual, nos, distancias, temperaturas

if __name__ == "__main__":

    if len(sys.argv) < 2:
        print("Uso: python programa.py <nome_do_arquivo>")
        sys.exit(1)

    # Obter o nome do arquivo do argumento da linha de comando
    arquivo = sys.argv[1]

    try:
        # Ler as cidades do arquivo
        cidades = ler_cidades(arquivo)
    except FileNotFoundError:
        print(f"Erro: O arquivo '{arquivo}' não foi encontrado.")
        sys.exit(1)
    except Exception as e:
        print(f"Erro ao ler o arquivo: {e}")
        sys.exit(1)
    
    grafo = scipy.spatial.distance_matrix(cidades, cidades)
    
    temperatura_inicial = 1000.0
    delta_t = 0.99
    limite_inferior_temperatura = 0.01
    
    inicio = time.time()
    caminho, custo_caminho, nos, distancias, temperaturas = caixeiro_viajante(
        grafo, cidades, temperatura_inicial, delta_t, limite_inferior_temperatura
    )
    fim = time.time()
    
    print(f"Distância total do caminho encontrado: {custo_caminho:.2f}")
    print(f"Tempo de execução: {fim - inicio:.2f} segundos")
    
    plotar_caminho(nos, caminho, custo_caminho)