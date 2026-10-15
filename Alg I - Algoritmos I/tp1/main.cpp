#include <iostream>
#include "graph.hpp"

using namespace std;

int main(int argc, char const *argv[]) {

    // Declara variaveis
    int num_cidades = 0;
    int num_estradas = 0;
    int cidade_a = 0;
    int cidade_b = 0;
    int distancia = 0;
    
    // Le o numero de cidades e estradas
    cin >> num_cidades >> num_estradas;

    // Inicializa o grafo que representa a rede de cidades
    Graph grafo(num_cidades);

    // Insere as estradas que ligam as cidades
    for (int i = 0; i < num_estradas; i++) {

        // Le cidade a, cidade b e distancia
        cin >> cidade_a >> cidade_b >> distancia;
        
        // Cria estrada entre as cidades
        grafo.AdicionaAresta(cidade_a, cidade_b, distancia);
    }

    // Calcula e imprime menor distancia entre as cidades de origem e de destino, atendendo as restricoes
    cout << grafo.CalculaMenorCaminho() << endl;

    return 0;
}