#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <limits>
#include "File.hpp"
#include "graph_search.hpp"

using namespace std;

int main(int argc, char* argv[]) {

    // Verifica se o número de argumentos é suficiente
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " <arquivo.txt> <modo (1 = saida padrao | 2 = arquivo)> (default = 2)" << endl;
        return 1;
    }

    bool mode_standard_output = false;
    if (argc == 3 && strcmp(argv[2], "1") == 0) {
        mode_standard_output = true;
    }

    // Abre o arquivo
    File file;
    if (!file.OpenFromCommandLine(argc, argv, File::READ)) {
        cerr << "Falha ao abrir o arquivo." << endl;
        return 1;
    }

    // Carrega o grafo
    Graph graph;
    if (!graph.LoadFromTxt(file)) {
        cerr << "Erro ao carregar o grafo do arquivo." << endl;
        return 1;
    }

    // Solicita origem e destino do usuário
    long long id_origin, id_destination;
    if (mode_standard_output) cout << "Digite o ID de origem: ";
    cin >> id_origin;
    if (mode_standard_output) cout << "Digite o ID de destino: ";
    cin >> id_destination;
    if (mode_standard_output) cout << endl;

    // Solicita o algoritmo ao usuário
    string algorithm;

    auto choose_algorithm = [&]() {
        while (true) {
            if (mode_standard_output) cout << "Escolha o algoritmo de busca (dijkstra ou astar): ";
            cin >> algorithm;
            if (mode_standard_output) cout << endl;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            if (algorithm == "dijkstra" || algorithm == "astar") break;
            if (mode_standard_output) cout << "Algoritmo invalido." << endl;
        }
    };

    choose_algorithm();

    // Verifica se os nós existem
    if (!graph.Exists(id_origin)) {
        cerr << "O no de origem nao existe no grafo." << endl;
        return 1;
    }
    if (!graph.Exists(id_destination)) {
        cerr << "O no de destino nao existe no grafo." << endl;
        return 1;
    }

    // Executa o algoritmo escolhido
    vector<Node> path;
    double distance = -1.0;
    if (algorithm == "dijkstra") {
        tie(path, distance) = graph.Dijkstra(id_origin, id_destination);
    } else if (algorithm == "astar") {
        tie(path, distance) = graph.AStar(id_origin, id_destination);
    }

    // Modo padrão: saída na tela
    if (mode_standard_output) {
        if (path.empty()) {
            cout << "Nao foi possivel encontrar um caminho entre os nos." << endl;
        } else {
            cout << "Caminho encontrado:" << endl << endl;
            for (const Node& node : path) {
                cout << "ID: " << node.id_original;
                if (!node.name.empty()) cout << " | Nome: " << node.name;
                if (!node.type.empty()) cout << " | Tipo: " << node.type;
                cout << " | Coordenadas: (" << node.x << ", " << node.y << ")" << endl;
            }
            cout << endl << "Distancia total: " << distance << endl;
        }
    } 
    
    else {
        // Modo integração: saída em arquivo
        ofstream out("../data/search_result.txt");
        if (!out.is_open()) {
            cerr << "Erro ao abrir o arquivo de saída search_result.txt." << endl;
            return 1;
        }

        if (path.empty()) {
            out << "-1" << endl;
        } else {
            out << distance << endl;
            for (const Node& node : path) {
                out << node.id_original << " "
                    << node.x << " "
                    << node.y << " "
                    << "\"" << node.name << "\" "
                    << "\"" << node.type << "\"" << endl;
            }
        }

        out.close();
    }

    return 0;
}