#ifndef INDICE
#define INDICE

#include "termo.hpp"
#include "hashing.hpp"
#include "filaencadeada.hpp"
#include "arq_funcao.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <dirent.h>
#include <cstdio>

using namespace std;

// Funcao que cria o indice invertido
void CriaIndice(string arq_consulta, string arq_ranking, string pasta_corpus, string arq_stopwords, int num_arq, int tam_vocabulario, Termo *indice);

#endif