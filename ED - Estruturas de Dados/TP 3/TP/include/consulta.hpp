#ifndef CONSULTA
#define CONSULTA

#include "termo.hpp"
#include "filaencadeada.hpp"
#include "similaridade.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

// Funcao que realiza a consulta
void Consulta(string arq_ranking, string arq_consulta, Termo indice[], int num_arq, int tam_vocabulario);

#endif