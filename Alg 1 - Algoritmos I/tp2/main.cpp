#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>

#define INF 0x3f3f3f3f

using namespace std;

class Graph {
    public:
        // Construtor
        Graph(int num_usuarios, int num_vagas);

        // Adiciona vertice
        void AdicionaVertice(string vertice, bool tipo);

        // Adicina aresta do vertice a ao vertice b e do b ao a
        void AdicionaAresta(string vertice_a, string vertice_b);

        // Retorna numero de pares de usuarios e vagas pelo algoritmo guloso
        int GetGuloso();

        // Retorna numero de pares de usuarios e vagas pelo algoritmo exato
        int GetExato();
    
    private:

        // Algoritmo guloso para encontrar pares unico de usuarios e vagas
        void AlgoritmoGuloso();

        // Algoritmo exato para encontrar pares unico de usuarios e vagas
        void AlgoritmoExato();

        // Função auxiliar para encontrar um caminho de aumento usando DFS
        bool DFS(int u, int t, vector<int>& pai);

        // Calcula a posicao do vertice v no vetor de adjacencia do vertice u
        int CalculaPosicao(int u, int v);
        
        // Numero de usuarios
        int num_usuarios;

        // Numero de vagas
        int num_vagas;

        // Numero total de vertices do grafo
        int num_vertices;

        // Contador de quantos vertices ja foram adicionados
        int count_vertices;

        // Armazena os id dos vertices
        unordered_map<string, int> id;

        // Armazena os vertices pelos id
        unordered_map<int, string> id_reverso;

        // Vetor de adjacencia dos vertices
        vector<vector<pair<int, int>>> arestas;
        
        // Vetor de distancias
        vector<int> distancias;

        // Armazena o tipo de vertice (usuario = true / vaga = false)
        vector<bool> tipo_vertice;

        // Numero de pares de usuarios e vagas pelo algoritmo guloso
        int guloso;

        // Numero de pares de usuarios e vagas pelo algoritmo exato
        int exato;
};

// Construtor
Graph::Graph(int num_usuarios, int num_vagas) {
    this->num_usuarios = num_usuarios;
    this->num_vagas = num_vagas;
    this->num_vertices = num_usuarios + num_vagas + 2;
    this->arestas.resize(num_usuarios + num_vagas + 2);
    this->tipo_vertice.resize(num_usuarios + num_vagas + 2);
    this->count_vertices = 1;
    this->id["Inicio"] = 0;
    this->id["Fim"] = num_usuarios + num_vagas + 1;
    this->id_reverso[0] = "Inicio";
    this->id_reverso[num_usuarios + num_vagas + 1] = "Fim";
}

// Adiciona vertice
void Graph::AdicionaVertice(string vertice, bool tipo) {
    if (this->id.find(vertice) == this->id.end()) { // Se vertice nao existe
        this->id[vertice] = this->count_vertices;
        this->id_reverso[this->count_vertices] = vertice;
        this->tipo_vertice[this->count_vertices] = tipo;
        this->count_vertices++;

        if (tipo)
            AdicionaAresta("Inicio", vertice);
        else 
            AdicionaAresta(vertice, "Fim");
    }
}

// Adiciona aresta do vertice a ao vertice b e do b ao a
void Graph::AdicionaAresta(string vertice_a, string vertice_b) {
    this->arestas[id[vertice_a]].push_back(make_pair(id[vertice_b], 1));
    this->arestas[id[vertice_b]].push_back(make_pair(id[vertice_a], 1));
}

// Retorna numero de pares de usuarios e vagas pelo algoritmo guloso
int Graph::GetGuloso() {
    AlgoritmoGuloso();
    return this->guloso;
}

// Retorna numero de pares de usuarios e vagas pelo algoritmo exato
int Graph::GetExato() {
    AlgoritmoExato();
    return this->exato;
}

// Algoritmo guloso para encontrar pares unico de usuarios e vagas
// Relaciona usuario e vaga pela ordem em que aparecem
void Graph::AlgoritmoGuloso() { 
    vector<bool> visitado(this->num_vertices, false);
    this->guloso = 0;
    for (int i = 1; i < this->num_vertices-1; i++) {
        if (this->tipo_vertice[i]) {
            for (int j = 1; j < this->arestas[i].size(); j++) {
                if (!visitado[this->arestas[i][j].first]) {
                    visitado[this->arestas[i][j].first] = true;
                    this->guloso++;
                    break;
                }   
            }   
        }
        if (this->guloso == this->num_usuarios || this->guloso == this->num_vagas)
            break;
    }   
}

// Algoritmo exato para encontrar pares unico de usuarios e vagas
// Utiliza o algoritmo de Ford-Fulkerson (Fluxo Maximo)
void Graph::AlgoritmoExato() {
    this->exato = 0;

    // Vetor para armazenar os vértices pai encontrados durante a busca
    vector<int> pai(this->num_vertices);

    // Enquanto houver um caminho de aumento do vértice de origem ao vértice de destino
    while (DFS(0, this->num_vertices-1, pai)) {
        
        int path_flow = INF;
        for (int v = this->num_vertices-1; v != 0; v = pai[v]) {
            int u = pai[v];
            int posicao_v = CalculaPosicao(u, v);
            path_flow = min(path_flow, this->arestas[u][posicao_v].second);
        }

        for (int v = this->num_vertices-1; v != 0; v = pai[v]) {
            int u = pai[v];
            int posicao_v = CalculaPosicao(u, v);
            int posicao_u = CalculaPosicao(v, u);
            this->arestas[u][posicao_v].second -= path_flow;
            this->arestas[v][posicao_u].second += path_flow;
        }

        this->exato += path_flow;
    }
}

// Função auxiliar para encontrar um caminho de aumento usando DFS
bool Graph::DFS(int inicio, int fim, vector<int>& pai) {
    vector<bool> visitado(this->num_vertices, false);
    visitado[inicio] = true;
    pai[inicio] = -1;

    queue<int> fila;
    fila.push(inicio);

    while (!fila.empty()) {
        int atual = fila.front();
        fila.pop();

        for (int i = 0; i < this->arestas[atual].size(); i++) {
            int v = this->arestas[atual][i].first;
            int capacidade = this->arestas[atual][i].second;

            if (!visitado[v] && capacidade > 0) {
                visitado[v] = true;
                pai[v] = atual;
                fila.push(v);
            }
        }
    }

    // Retorna true se existir um caminho de aumento até o vértice de destino
    return visitado[fim];
}

// Calcula a posicao do vertice v no vetor de adjacencia do vertice u
int Graph::CalculaPosicao(int u, int v) {
    int posicao = 0;

    for (int i = 0; i < this->arestas[u].size(); i++) {
        if (this->arestas[u][i].first == v) {
            posicao = i;
            break;   
        }
    }
    
    return posicao;
}

int main(int argc, char const *argv[]) {

    int num_usuarios = 0;
    int num_vagas = 0;
    int num_qualificacoes_usuario_emprego = 0;
    string usuario;
    string vaga;

    cin >> num_usuarios >> num_vagas >> num_qualificacoes_usuario_emprego;

    Graph grafo(num_usuarios, num_vagas);

    for (int i = 0; i < num_qualificacoes_usuario_emprego; i++) {
        cin >> usuario >> vaga;
        grafo.AdicionaVertice(usuario, true);
        grafo.AdicionaVertice(vaga, false);
        grafo.AdicionaAresta(usuario, vaga);
    }

    cout << "Guloso: " << grafo.GetGuloso() << endl << "Exato: " << grafo.GetExato();

    return 0;
}