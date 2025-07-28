#ifndef GRAPH_SEARCH_HPP
#define GRAPH_SEARCH_HPP

#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <utility>
#include "File.hpp"

#define INF 0x3f3f3f3f

using namespace std;

struct Node {
    int id_internal; // índice interno (usado para busca e adjacência)
    long long id_original;   // id original do OSM
    double x, y;
    string name = "";
    string type = "";
};

class Graph {
    public:
        // Construtor
        Graph();

        // Adiciona um nó ao grafo
        void AddNode(long long id_original, double x, double y, const string& name, const string& type);

        // Adicina aresta do vertice a ao vertice b
        void AddEdge(long long node_a, long long node_b, double length);

        // Lê o grafo a partir de um arquivo .txt
        bool LoadFromTxt(File& file);

        // Verifica se o id_original existe no grafo
        bool Exists(long long id_original) const;

        // Dijkstra Search
        pair<vector<Node>, double> Dijkstra(long long original_start, long long original_end);

        // A* Search
        pair<vector<Node>, double> AStar(long long original_start, long long original_end);
    
    private:
        
        // Numero de vertices do grafo
        int num_nodes;

        // Nós do grafo
        vector<Node> nodes_info; // index -> Node

        // Vetor de adjacencia dos vertices
        vector<vector<pair<int, double>>> nodes_adjacency; // index -> (node, length)

        // Mapeia id_original para id_intern
        unordered_map<long long, int> original_to_internal; // id_original -> id_internal
        
        // Calcula a distancia euclidiana entre dois pontos
        double Heuristic(int a, int b);

};

#endif // GRAPH_SEARCH_HPP