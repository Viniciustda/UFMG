#ifndef QUICKSORT
#define QUICKSORT

#include "entidade.hpp"
#include <iostream>
#include <string>

using namespace std;

void VerificaOrdemAlfabetica (Entidade *A, int tam);

int MedianaDeTres(int a, int b, int c);

void Particao(int Esq, int Dir, int *i, int *j, Entidade *A);

void Ordena(int Esq, int Dir, Entidade *A);

void QuickSort(Entidade *A, int n);

#endif