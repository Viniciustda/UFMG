#include "graph.hpp"
#include <iostream>

using namespace std;

// Construtor
Graph::Graph(int num_vertices) {
    this->num_vertices = num_vertices;
    num_vertices *= 2; // Para implementacao usaremos o dobro de vertices
    num_vertices++; // Nao existe o vertice 0
    this->vertices.resize(num_vertices);
    this->distancias.resize(num_vertices);
    for (int i = 0; i < num_vertices; i++)  
        this->distancias[i] = INF;
}

// Adicina aresta do vertice a ao vertice b, lembrando que nao eh um grafo direcionado
void Graph::AdicionaAresta(int vertice_a, int vertice_b, int tamanho) {
    if (tamanho % 2 == 0) { // Se a aresta for de tamanho impar, ela eh ignorada
        /* 
        Usaremos os vertices impares e os pares, separados cada tipo em uma metade do vetor de vertices
        Ex.: vertices[3] = [0][vertice_a par][vertice_b par][vertice_c par][vertice_a impar][vertice_b impar][vertice_c impar]
        Os vertices pares e impares se conectam de forma intercaladas
        Ex.: vertice_a par - vertice_b impar 
             vertice_a impar - vertice_b par
             Obs.: O grafo eh nao direcional, logo tambem eh feita a ligacao de vertice_b para vertice_a da mesma forma
        */
        this->vertices[vertice_a].push_back(make_pair(vertice_b+this->num_vertices, tamanho));
        this->vertices[vertice_b].push_back(make_pair(vertice_a+this->num_vertices, tamanho));
        this->vertices[vertice_a+this->num_vertices].push_back(make_pair(vertice_b, tamanho));
        this->vertices[vertice_b+this->num_vertices].push_back(make_pair(vertice_a, tamanho));
    }
}

// Dijkstra
int Graph::CalculaMenorCaminho() {

    int menor_distancia = INF;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> fila_prioridade; // A fila de prioridade implementa um heap interno

    fila_prioridade.push(make_pair(0, 1)); // Vertice 1 tem distancia 0 para ele mesmo
    this->distancias[1] = 0;
    
    /*
    Enquanto a fila de prioridade/heap nao estiver vazio
        vertice v = vertice do topo do heap
        exclui vertice do topo do heap
        para cada aresta que liga o vertice v a um vertice w
            vertice w = vertice ligado ao vertice v
            tamanho = tamanho/peso da aresta que liga vertices v e w
            se a distancia do vertice w ao vertice inicial for maior que (a distancia do vertice v ao vertice inicial + o tamanho da aresta v-w) 
                atualiza a distancia do vertice w
                insere a distancia do vertice w ao vertice inicial na fila de prioridade
    */

    while (!fila_prioridade.empty()) {
        int vertice_v = fila_prioridade.top().second;
        fila_prioridade.pop();
        for (auto i = this->vertices[vertice_v].begin(); i != this->vertices[vertice_v].end(); i++) {
            int vertice_w = (*i).first;
            int tamanho = (*i).second;
            if ((this->distancias[vertice_w] > this->distancias[vertice_v] + tamanho)) {
                this->distancias[vertice_w] = this->distancias[vertice_v] + tamanho;
                fila_prioridade.push(make_pair(this->distancias[vertice_w], vertice_w));
            }
        }
    }

    // Caso exista um caminho que atenda as condicoes
    if (menor_distancia > this->distancias[this->num_vertices])
        menor_distancia = this->distancias[this->num_vertices];
    
    // Caso nao exista um caminho que atenda as condicoes, sera retornado -1
    if (menor_distancia == INF)      
        menor_distancia = -1;

    return menor_distancia;
}