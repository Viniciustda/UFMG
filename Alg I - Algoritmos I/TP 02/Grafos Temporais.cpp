#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>

using namespace std;

#define INF 0x7FFFFFFFFFFFFFFF

struct Conexao {
    long long int origem;
    long long int destino;
    long long int ano;
    long long int tempo;
    long long int custo;
};


class Graph {
    public:
        // Construtor
        Graph(long long int num_vertices);

        // Adicina aresta do vertice a ao vertice b
        void AdicionaAresta(long long int vertice_a, long long int vertice_b, long long int ano, long long int tempo, long long int custo);


        // Imprime as informacoes pedidas pelo Conselho
        void ImprimeDados();

    private:

        // Dijkstra - calcula tempo minimo entre o palacio real e todas vilas
        void TempoMinimo();

        // Kruskal - calcula primeiro ano em que todo o reino eh alcancavel a partir do palacio real
        void PalacioAlcancaReino();

        // Kruskal - calcula menor custo necessario para conectar todo o reino
        void MenorCusto();

        // Numero de vertices do grafo original
        long long int num_vertices;

        // Vetor de adjacencia dos vertices
        vector<vector<Conexao>> vertices;

        // Vetor com tempo minimo necessario para sair do palacio real e chegar nas vilas
        vector<long long int> tempo_minimo;

        // Primeiro ano em que todas as distancias podem ser realizadas ao mesmo tempo
        long long int dist_mesmo_tempo;

        // Primeiro ano em que todo o reino eh alcancavel a partir do palacio real
        long long int palacio_alcanca_reino;

        // Menor custo necessario para conectar todo o reino
        long long int menor_custo;
};

// Construtor
Graph::Graph(long long int num_vertices) {
    this->num_vertices = num_vertices;
    num_vertices++; // Nao existe o vertice 0
    this->vertices.resize(num_vertices);
    this->tempo_minimo.resize(num_vertices);
    for (long long int i = 0; i < num_vertices; i++)  
        this->tempo_minimo[i] = INF;
    this->dist_mesmo_tempo = 0;
    this->palacio_alcanca_reino = 0;
    this->menor_custo = 0;
}

// Adicina aresta do vertice a ao vertice b, lembrando que nao eh um grafo direcionado
void Graph::AdicionaAresta(long long int vertice_a, long long int vertice_b, long long int ano, long long int tempo, long long int custo) {
    Conexao aux_a{vertice_a, vertice_b, ano, tempo, custo};
    Conexao aux_b{vertice_b, vertice_a, ano, tempo, custo};
    this->vertices[vertice_a].push_back(aux_a);
    this->vertices[vertice_b].push_back(aux_b);
}

// Dijkstra - calcula tempo minimo entre o palacio real e todas vilas
void Graph::TempoMinimo() {

    priority_queue<pair<long long int, long long int>, vector<pair<long long int, long long int>>, greater<pair<long long int, long long int>>> fila_prioridade; // A fila de prioridade implementa um heap interno

    vector<long long int> ano_construcao(this->num_vertices + 1, -1); // Vetor para armazenar o ano de construção mais tardio para cada vila
    ano_construcao[0] = 0; // Nao existe vertice 0
    ano_construcao[1] = 0; // O ano de construção para a vila de origem é 0

    fila_prioridade.emplace(0, 1); // Vertice 1 tem distancia 0 para ele mesmo
    this->tempo_minimo[1] = 0;

    while (!fila_prioridade.empty()) {
        long long int vertice_v = fila_prioridade.top().second;
        fila_prioridade.pop();

        for (auto i = this->vertices[vertice_v].begin(); i != this->vertices[vertice_v].end(); i++) {
            long long int vertice_w = (*i).destino;
            long long int tamanho = (*i).tempo;
            long long int ano = (*i).ano;

            if (this->tempo_minimo[vertice_w] > this->tempo_minimo[vertice_v] + tamanho) {
                this->tempo_minimo[vertice_w] = this->tempo_minimo[vertice_v] + tamanho;
                ano_construcao[vertice_w] = max(ano_construcao[vertice_v], ano);
                fila_prioridade.emplace(this->tempo_minimo[vertice_w], vertice_w);
                
            } else if (this->tempo_minimo[vertice_w] == this->tempo_minimo[vertice_v] + tamanho) {
                ano_construcao[vertice_w] = min(ano_construcao[vertice_w], max(ano_construcao[vertice_v], ano));
                fila_prioridade.emplace(this->tempo_minimo[vertice_w], vertice_w);
            }
        }
    }

    for (long long int i = 0; i < ano_construcao.size(); i++) {
        if (this->dist_mesmo_tempo < ano_construcao[i]) {
            this->dist_mesmo_tempo = ano_construcao[i];
        }
    }
}

// Kruskal - calcula primeiro ano em que todo o reino eh alcancavel a partir do palacio real
void Graph::PalacioAlcancaReino() {

    vector<Conexao> arestas;
    for (long long int i = 1; i <= this->num_vertices; i++) {
        for (auto j = this->vertices[i].begin(); j != this->vertices[i].end(); j++) {
            arestas.push_back(*j);
        }
    }

    sort(arestas.begin(), arestas.end(), [](Conexao a, Conexao b) {
        return a.ano < b.ano;
    });

    vector<long long int> representante(this->num_vertices+1);
    for (long long int i = 1; i <= this->num_vertices; i++) {
        representante[i] = i;
    }

    vector<long long int> tam_conjunto(this->num_vertices+1, 1);

    function<long long int(long long int)> find;
    find = [&](long long int a) { 
        return a == representante[a] ? a : representante[a] = find(representante[a]); 
    };

    auto unite = [&](long long int a, long long int b) {
		a = find(a), b = find(b);
		if (a == b) return;
		if (tam_conjunto[a] < tam_conjunto[b]) swap(a, b);
		tam_conjunto[a] += tam_conjunto[b], representante[b] = a;
	};

    long long int ano = 0;
    for (auto conexao = arestas.begin(); conexao != arestas.end(); conexao++) {
        if (find((*conexao).origem) != find((*conexao).destino)) {
            ano += (*conexao).ano;
            unite((*conexao).origem, (*conexao).destino);
            if (this->palacio_alcanca_reino < (*conexao).ano) {
                this->palacio_alcanca_reino = (*conexao).ano;
            }
            
        }
    }
}

// Kruskal - calcula menor custo necessario para conectar todo o reino
void Graph::MenorCusto() {

    vector<Conexao> arestas;
    for (long long int i = 1; i <= this->num_vertices; i++) {
        for (auto j = this->vertices[i].begin(); j != this->vertices[i].end(); j++) {
            arestas.push_back(*j);
        }
    }

    sort(arestas.begin(), arestas.end(), [](Conexao a, Conexao b) {
        return a.custo < b.custo;
    });

    vector<long long int> representante(this->num_vertices+1);
    for (long long int i = 1; i <= this->num_vertices; i++) {
        representante[i] = i;
    }

    vector<long long int> tam_conjunto(this->num_vertices+1, 1);

    function<long long int(long long int)> find;
    find = [&](long long int a) { 
        return a == representante[a] ? a : representante[a] = find(representante[a]); 
    };

    auto unite = [&](long long int a, long long int b) {
    a = find(a), b = find(b);
    if (a == b) return;
    if (tam_conjunto[a] < tam_conjunto[b]) swap(a, b);
    tam_conjunto[a] += tam_conjunto[b], representante[b] = a;
  };

    long long int custo = 0;
    for (auto conexao = arestas.begin(); conexao != arestas.end(); conexao++) {
        if (find((*conexao).origem) != find((*conexao).destino)) {
            custo += (*conexao).custo;
            unite((*conexao).origem, (*conexao).destino);
        }
    }

    this->menor_custo = custo;
}

// Imprime as informacoes pedidas pelo Conselho
void Graph::ImprimeDados() {

    this->TempoMinimo();
    for (long long int i = 1; i <= this->num_vertices; i++) 
        printf("%lld\n", this->tempo_minimo[i]);

    printf("%lld\n", this->dist_mesmo_tempo);

    this->PalacioAlcancaReino();
    printf("%lld\n", this->palacio_alcanca_reino);

    this->MenorCusto();
    printf("%lld\n", this->menor_custo);
}

int main(int argc, char const *argv[]) {

    // Declara variaveis
    long long int num_vilas = 0;
    long long int num_conexoes = 0;
    long long int vila_a = 0;
    long long int vila_b = 0;
    long long int ano = 0;
    long long int tempo_travessia = 0;
    long long int custo_construcao = 0;

    // Le o numero de vilas e conexoes
    scanf("%lld %lld", &num_vilas, &num_conexoes);

    // Inicializa o grafo que representa a rede de canais e estradas
    Graph grafo(num_vilas);

    // Insere as estradas que ligam as vilas
    for (long long int i = 0; i < num_conexoes; i++) {

        // Le vila a, vila b, ano em que a conexao sera finalizada, tempo de travessia e custo de construcao
        scanf("%lld %lld %lld %lld %lld", &vila_a, &vila_b, &ano, &tempo_travessia, &custo_construcao);

        // Cria conexao entre as vilas
        grafo.AdicionaAresta(vila_a, vila_b, ano, tempo_travessia, custo_construcao);
    }

    // Imprime as informacoes pedidas pelo Conselho
    grafo.ImprimeDados();

    return 0;
}