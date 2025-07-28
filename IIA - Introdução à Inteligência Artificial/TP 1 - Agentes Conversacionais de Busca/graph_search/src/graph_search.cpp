#include "graph_search.hpp"
#include "File.hpp"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm> 
#include <cmath>

using namespace std;

// Construtor
Graph::Graph() {
    this->num_nodes = 0;
}

// Adiciona um nó ao grafo
void Graph::AddNode(long long id_original, double x, double y, const string& name, const string& type) {
    
    // ID interno é o número de nós atuais
    int id_internal = this->num_nodes;
    this->num_nodes++;

    // Cria o nó
    Node node = {id_internal, id_original, x, y, name, type};

    // Adiciona o nó à lista de nós
    nodes_info.push_back(node);
    
    // Mapeamento do id_original para id_internal
    original_to_internal[id_original] = id_internal; 

    // Inicializa o vetor de adjacência para o novo nó
    nodes_adjacency.emplace_back(); 
}

// Adicina aresta do vertice a ao vertice b
void Graph::AddEdge(long long original_node_a, long long original_node_b, double length) {

    // Verifica se os nós existem
    if (!Exists(original_node_a)) {
        cerr << "Aresta ignorada: " << original_node_a << " -> " << original_node_b << " (no " << original_node_a << " inexistente)\n";
        return;
    }
    // Verifica se os nós existem
    if (!Exists(original_node_b)) {
        cerr << "Aresta ignorada: " << original_node_a << " -> " << original_node_b << " (no " << original_node_b << " inexistente)\n";
        return;
    }

    // Mapeia os ids originais para os ids internos
    int internal_a = original_to_internal.at(original_node_a);
    int internal_b = original_to_internal.at(original_node_b);

    // Grafo não direcionado: adiciona aresta em ambos os sentidos, mas o txt tem os dois sentidos
    nodes_adjacency[internal_a].push_back(make_pair(internal_b, length));
}

// Lê o grafo a partir de um arquivo .txt
bool Graph::LoadFromTxt(File& file) {
    if (!file.IsOpen()) return false;

    enum Section { NONE, NODES, EDGES };
    Section current_section = NONE;

    string line;
    while (getline(file.GetStream(), line)) {

        // Ignora linhas em branco e comentários (linhas que começam com '#')
        if (line.empty() || line[0] == '#') {
            if (line.find("# NODES") != string::npos) {
                current_section = NODES;
            } else if (line.find("# EDGES") != string::npos) {
                current_section = EDGES;
            }
            else if (line.empty() && current_section != NONE) {
                current_section = NONE; 
            }
            continue;
        }

        // Ignora cabeçalhos
        if (line == "id x y name type" || line == "from_id to_id length") {
            continue;
        }

        if (current_section == NODES) {
            int i = 0;
            const int len = line.size();
           
            try {
                // Campo 1: id
                while (i < len && isspace(line[i])) i++;
                int id_start = i;
                while (i < len && !isspace(line[i])) i++;
                long long id = stoll(line.substr(id_start, i - id_start));

                // Campo 2: x
                while (i < len && isspace(line[i])) i++;
                int x_start = i;
                while (i < len && !isspace(line[i])) i++;
                double x = stod(line.substr(x_start, i - x_start));
            
                // Campo 3: y
                while (i < len && isspace(line[i])) i++;
                int y_start = i;
                while (i < len && !isspace(line[i])) i++;
                double y = stod(line.substr(y_start, i - y_start));
            
                // Campo 4: name (entre aspas)
                while (i < len && line[i] != '"') i++; 
                i++; // pula a primeira aspas
                int name_start = i;
                while (i < len && line[i] != '"') i++;
                string name = line.substr(name_start, i - name_start);
                i++; // pula a aspas final
            
                // Campo 5: type (entre aspas)
                while (i < len && line[i] != '"') i++; 
                i++; // pula a primeira aspas
                int type_start = i;
                while (i < len && line[i] != '"') i++;
                string type = line.substr(type_start, i - type_start);
                // não precisa pular a última aspas pois acabou a linha
                
                AddNode(id, x, y, name, type);

            } catch (const exception& e) {
                cerr << "Erro ao converter linha: " << line << "\n" << e.what() << endl;
                continue;
            }

        } else if (current_section == EDGES) {
            // Leitura das arestas (from_id, to_id, length)
            istringstream iss(line);
            long long from_id, to_id;
            double length;
            iss >> from_id >> to_id >> length;
            AddEdge(from_id, to_id, length);  // Adiciona a aresta no grafo
        }
    }

    return true;
}

// Verifica se o id_original existe no grafo
bool Graph::Graph::Exists(long long id_original) const {
    return original_to_internal.find(id_original) != original_to_internal.end();
}

// Dijkstra Search
pair<vector<Node>, double> Graph::Dijkstra(long long original_start, long long original_end) {

    int start = original_to_internal.at(original_start);
    int end = original_to_internal.at(original_end);

    // Vetor que armazena a menor distância do nó inicial até cada nó
    vector<double> distances(num_nodes, INF);

    // Vetor que armazena o nó anterior no caminho mais curto até cada nó
    vector<int> previous(num_nodes, -1);

    // Vetor que marca nós já visitados
    vector<bool> visited(num_nodes, false);

    // Fila de prioridade implementa um heap interno onde cada elemento é um par (distância, id_do_nó)
    // O nó com a menor distância conhecida será sempre processado primeiro
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    // A distância do nó inicial para ele mesmo é 0
    distances[start] = 0.0;

    // Adiciona o nó inicial à fila de prioridade
    pq.push({0.0, start});

    while (!pq.empty()) {
        // Obtém o nó com menor distância atual
        auto [current_distance, current_node] = pq.top();
        pq.pop();

        // Ignora nós que já foram processados
        if (visited[current_node]) continue;
        visited[current_node] = true;

        // Se o destino foi alcançado, pode encerrar a busca
        if (current_node == end) break;

        // Percorre todos os vizinhos do nó atual
        for (auto [neighbor_node, length] : nodes_adjacency[current_node]) {

            // Se a nova distância for menor que a anterior conhecida, atualiza
            if (distances[neighbor_node] > distances[current_node] + length) {
                distances[neighbor_node] = distances[current_node] + length;
                previous[neighbor_node] = current_node;

                // Adiciona o vizinho à fila de prioridade com a nova distância
                pq.push({distances[neighbor_node], neighbor_node});
            }
        }
    }

    // Reconstrução do caminho
    vector<Node> path;
    if (distances[end] == INF) return {path, INF}; // Sem caminho

    // Caminho é reconstruído de trás para frente
    for (int actual = end; actual != -1; actual = previous[actual]) {
        path.push_back(nodes_info[actual]);
    }

    // O caminho foi construído ao contrário, então é necessário inverter
    reverse(path.begin(), path.end());

    return {path, distances[end]};
}

// Heurística: distância euclidiana entre dois nós (usada no A*)
double Graph::Heuristic(int a, int b) {
    double dx = nodes_info[a].x - nodes_info[b].x;
    double dy = nodes_info[a].y - nodes_info[b].y;
    return sqrt(dx * dx + dy * dy);
}

// A* Search
pair<vector<Node>, double> Graph::AStar(long long original_start, long long original_end) {

    int start = original_to_internal.at(original_start);
    int end = original_to_internal.at(original_end);

    // Vetor que armazena a menor distância do nó inicial até cada nó
    vector<double> distances(num_nodes, INF);

    // Vetor que armazena o custo total estimado do início até o destino passando por cada nó (f(n) = g(n) + h(n))
    vector<double> estimated_total_cost(num_nodes, INF);

    // Vetor que armazena o nó anterior no caminho mais curto até cada nó
    vector<int> previous(num_nodes, -1);

    // Fila de prioridade ordenada por f(n) = g(n) + h(n)
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    // Inicializa o nó inicial
    distances[start] = 0.0;
    estimated_total_cost[start] = Heuristic(start, end);

    // Adiciona o nó inicial à fila de prioridade
    pq.push({estimated_total_cost[start], start});

    while (!pq.empty()) {
        // Obtém o nó com menor distância atual
        auto [estimated_cost, current_node] = pq.top();
        pq.pop();

        // Se o destino foi alcançado, pode encerrar a busca
        if (current_node == end) break;

        // Percorre todos os vizinhos do nó atual
        for (auto [neighbor_node, length] : nodes_adjacency[current_node]) {

            // Calcula o novo custo g(n)
            double new_distance = distances[current_node] + length;

            // Se a nova distância for menor que a anterior conhecida, atualiza
            if (distances[neighbor_node] > new_distance) {
                distances[neighbor_node] = new_distance;
                estimated_total_cost[neighbor_node] = new_distance + Heuristic(neighbor_node, end);
                previous[neighbor_node] = current_node;

                // Adiciona o vizinho à fila de prioridade com a nova distância
                pq.push({estimated_total_cost[neighbor_node], neighbor_node});
            }
        }
    }

    // Reconstrução do caminho
    vector<Node> path;
    if (distances[end] == INF) return {path, INF}; // Sem caminho

    // Caminho é reconstruído de trás para frente
    for (int actual = end; actual != -1; actual = previous[actual]) {
        path.push_back(nodes_info[actual]);
    }

    // O caminho foi construído ao contrário, então é necessário inverter
    reverse(path.begin(), path.end());

    return {path, distances[end]};
}