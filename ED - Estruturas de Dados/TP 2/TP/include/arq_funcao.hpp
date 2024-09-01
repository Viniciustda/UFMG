#ifndef ARQ_FUNCAO
#define ARQ_FUNCAO

#include "filaencadeada.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Confere se o arquivo esta vazio
bool ArquivoVazio(string nome_arq);

// Apaga primeira linha do arquivo
void ApagaPrimeiraLinhaArquivo (string nome_arq);

// Conta quantas linhas tem no arquivo
int ContaLinhas(string nome_arq_entrada);

#endif