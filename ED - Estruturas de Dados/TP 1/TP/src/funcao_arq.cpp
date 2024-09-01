#include "funcao_arq.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//confere se o arquivo esta vazio
bool ArquivoVazio(string nome_arq) {
    bool vazio = false;
    string linha;
    int indice = 0;
    ifstream arquivo_saida;
    arquivo_saida.open(nome_arq);

    while(!arquivo_saida.eof()) {
        getline(arquivo_saida, linha);
        indice++;
    }
    if (indice == 1 && linha == "") 
        vazio = true;
    
    arquivo_saida.close();
    return vazio;
}