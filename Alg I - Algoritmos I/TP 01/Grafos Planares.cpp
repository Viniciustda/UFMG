#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

struct Ponto {
    double x, y;
};

// Distancia euclidiana de a para b.
double Distancia(Ponto a, Ponto b) {
  double x = (a.x - b.x), y = (a.y - b.y);
  return sqrt(x*x + y*y);
}

// Coeficiente da reta que passa na origem e p.
double Inclinacao(Ponto p) {
  return atan2(p.y, p.x);
}

// Coeficiente da reta orientada de p para q.
double InclinacaoRelativa(Ponto p, Ponto q) {
  return atan2(q.y - p.y, q.x - p.x);
}

// Determina se ao caminhar de a para b e depois de b para c estamos fazendo uma curva a esquerda, a direita, ou seguindo em frente.
int TipoCurva(Ponto a, Ponto b, Ponto c) {
  double v = a.x*(b.y-c.y)+b.x*(c.y-a.y)+c.x*(a.y-b.y);
  if (v < 0) return -1; // esquerda.
  if (v > 0) return +1; // direita.
  return 0; // em frente.
}

class Graph {
    public:
        // Construtor
        Graph(int num_vertices, int num_arestas);

        // Retorna numero de faces do grafo
        int GetNumFaces();

        // Adiciona aresta do vertice a ao vertice b
        void AdicionaAresta(int vertice_a, int vertice_b);

        // Armazena as coordenadas do vertice
        void ArmazenaCoordenadas(int vertice, double x, double y);

        // Imprime as faces do grafo
        void ImprimeFaces();
    
    private:
        
        // Numero de vertices do grafo
        int num_vertices;

        // Numero de arestas do grafo
        int num_arestas;

        // Numero de faces do grafo
        int num_faces;

        // Vetor de adjacencia dos vertices
        vector<vector<int>> arestas;

        // Vetor com as coordenadas dos vertices
        vector<Ponto> vertices;

        // Vetor com as faces do grafo
        vector<vector<int>> faces; 

        // Ordena vizinhos do vertice, com base nas suas coordenadas em relacao ao vertice principal
        void OrdenaVizinhos(int indice);

        // Descobre as faces do grafo
        void DescobreFaces();
};

// Construtor
Graph::Graph(int num_vertices, int num_arestas) {
    this->num_vertices = num_vertices;
    this->num_arestas = num_arestas;
    this->num_faces = 2 + num_arestas - num_vertices;
    num_vertices++; // Nao existe o vertice 0
    this->arestas.resize(num_vertices);
    this->vertices.resize(num_vertices);
}

// Retorna numero de faces do grafo
int Graph::GetNumFaces() {
    return this->num_faces;
}

// Adiciona aresta do vertice a ao vertice b
void Graph::AdicionaAresta(int vertice_a, int vertice_b) {
    this->arestas[vertice_a].push_back(vertice_b);
}

// Armazena as coordenadas do vertice
void Graph::ArmazenaCoordenadas(int vertice, double x, double y) {
    this->vertices[vertice].x = x;
    this->vertices[vertice].y = y;
}

// Ordena vizinhos do vertice, com base nas suas coordenadas em relacao ao vertice principal
void Graph::OrdenaVizinhos(int indice) {
    auto ComparaVizinhos = [&](int a, int b) {

        Ponto p; 
        p.x = this->vertices[a].x - this->vertices[indice].x;
        p.y = this->vertices[a].y - this->vertices[indice].y;

        Ponto q; 
        q.x = this->vertices[b].x - this->vertices[indice].x;
        q.y = this->vertices[b].y - this->vertices[indice].y;

        double inclinacao_p = Inclinacao(p);
        double inclinacao_q = Inclinacao(q);

        // Comparação com base na inclinação relativa
        if (inclinacao_p != inclinacao_q)
            return inclinacao_p < inclinacao_q;

        // Se a inclinação for a mesma, comparar com base no tipo de curva
        return TipoCurva(vertices[indice], vertices[a], vertices[b]) > 0;
    };
    sort(this->arestas[indice].begin(), this->arestas[indice].end(), ComparaVizinhos);
}

// Descobre as faces do grafo
void Graph::DescobreFaces() {
    vector<vector<bool>> visitados(this->num_vertices+1);

    for (int i = 1; i <= this->num_vertices; i++) {
        visitados[i].resize(this->arestas[i].size(), false);
        this->OrdenaVizinhos(i);
    }
    
    for (int i = 1; i <= this->num_vertices; i++) {
        for (int j = 0; j < this->arestas[i].size(); j++) {
            if (visitados[i][j]) continue;

            vector<int> face;
            int vertice = i;
            int aresta = j;

            while(!visitados[vertice][aresta]) {
              
                visitados[vertice][aresta] = true;
                face.push_back(vertice);

                int vertice_adj = this->arestas[vertice][aresta];
                int prox_aresta = lower_bound(this->arestas[vertice_adj].begin(), this->arestas[vertice_adj].end(), vertice, [&](int a, int b) {
                  
                    Ponto p; 
                    p.x = this->vertices[a].x - this->vertices[vertice_adj].x;
                    p.y = this->vertices[a].y - this->vertices[vertice_adj].y;

                    Ponto q; 
                    q.x = this->vertices[b].x - this->vertices[vertice_adj].x;
                    q.y = this->vertices[b].y - this->vertices[vertice_adj].y;

                    double inclinacao_p = Inclinacao(p);
                    double inclinacao_q = Inclinacao(q);
            
                    // Comparação com base na inclinação relativa
                    if (inclinacao_p != inclinacao_q)
                        return inclinacao_p < inclinacao_q;
            
                    // Se a inclinação for a mesma, comparar com base no tipo de curva
                    return TipoCurva(vertices[vertice_adj], vertices[a], vertices[b]) > 0;
                  
                })- this->arestas[vertice_adj].begin() + 1;
              
                if (prox_aresta == this->arestas[vertice_adj].size()) {
                    prox_aresta = 0;
                }
                vertice = vertice_adj;
                aresta = prox_aresta;
            }
            face.push_back(face[0]);
            this->faces.push_back(face);
        }
    }
}

// Imprime as faces do grafo
void Graph::ImprimeFaces() {
    this->DescobreFaces();
    for (int i = 0; i < this->num_faces; i++) {
        cout << endl << this->faces[i].size() << " ";
        for (int j = 0; j < this->faces[i].size(); j++) {
            cout << this->faces[i][j] << " ";
        }
    }
}

int main(int argc, char const *argv[]) {

    // Variaveis
    int num_vertices = 0;
    int num_arestas = 0;
    int num_faces = 0;

    double coordenada_x = 0;
    double coordenada_y = 0;
    int grau_vertice = 0;
    int vizinho = 0;

    // Define numero de vertices e arestas do grafo
    cin >> num_vertices >> num_arestas;

    // Cria o grafo
    Graph grafo(num_vertices, num_arestas);

    for (int i = 1; i <= num_vertices; i++) {
        cin >> coordenada_x >> coordenada_y >> grau_vertice;

        grafo.ArmazenaCoordenadas(i, coordenada_x, coordenada_y);

        for (int j = 0; j < grau_vertice; j++) {
            cin >> vizinho;
            grafo.AdicionaAresta(i, vizinho);
        }
    }

    // Imprime as faces do grafo
    cout << grafo.GetNumFaces();
    grafo.ImprimeFaces();
    
    return 0;
}