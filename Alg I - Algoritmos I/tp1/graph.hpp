#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <queue>

#define INF 0x3f3f3f3f

using namespace std;

class Graph {
    public:
        // Construtor
        Graph(int num_vertices);

        // Adicina aresta do vertice a ao vertice b
        void AdicionaAresta(int vertice_a, int vertice_b, int tamanho);

        // Dijkstra
        int CalculaMenorCaminho();
    
    private:
        
        // Numero de vertices do grafo original
        int num_vertices;

        // Vetor de adjacencia dos vertices
        vector<vector<pair<int, int>>> vertices;
        
        // Vetor de distancias
        vector<int> distancias;
};

#endif // GRAPH_HPP