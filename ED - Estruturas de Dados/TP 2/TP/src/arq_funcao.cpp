#include "arq_funcao.hpp"
#include "filaencadeada.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Confere se o arquivo esta vazio
bool ArquivoVazio(string nome_arq) {
    bool vazio = false;
    string linha;
    int indice = 0;
    ifstream arquivo_saida;
    arquivo_saida.open(nome_arq);

    while(!arquivo_saida.eof()) {
        getline(arquivo_saida, linha);
        indice++;
        if (indice > 1)
            break;
    }
    if (indice == 1 && linha == "") 
        vazio = true;
    
    arquivo_saida.close();
    return vazio;
}

// Apaga primeira linha do arquivo
void ApagaPrimeiraLinhaArquivo (string nome_arq) {

    FilaEncadeada linhas;
    string entrada;
    string saida;

    ifstream arquivo_entrada;
    arquivo_entrada.open(nome_arq);
    if (! arquivo_entrada) {
        cout << "Arquivo de entrada nao pode ser aberto" << endl;
        abort();
    }

    getline(arquivo_entrada, entrada);
    while (!arquivo_entrada.eof()) {
        getline(arquivo_entrada, entrada);
        if (entrada != "")
            linhas.Enfileira(entrada);
    }

    arquivo_entrada.close();

    ofstream arquivo_saida;
    arquivo_saida.open(nome_arq);
    if (! arquivo_saida) {
        cout << "Arquivo de saida nao pode ser aberto (2)" << endl;
        abort();
    }

    while (!linhas.Vazia()) {
        saida = linhas.Desenfileira();
        if (saida != " " && saida != "\n" && saida != "\0")
            arquivo_saida << saida << endl;
    }
    
    arquivo_saida.close();

}

int ContaLinhas(string nome_arq_entrada) {
    string linha;
    int count = 0;

    // Abre o arquivo de entrada
    // Passado pela linha de comando
    ifstream arquivo_entrada;
    arquivo_entrada.open(nome_arq_entrada);
    if (! arquivo_entrada) {
        cout << "Arquivo de entrada nao pode ser aberto" << endl;
        abort();
    }

    // Conta quantas entidades tem no arquivo
    while (!arquivo_entrada.eof()) {
        getline(arquivo_entrada, linha);
        if (linha != "")
            count++;
    }
        
    // Fecha o arquivo
    arquivo_entrada.close();

    return count;
}