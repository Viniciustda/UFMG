#ifndef HEAP
#define HEAP

#include "entidade.hpp"
#include <iostream>
#include <string>

using namespace std;

void Constroi(Entidade *A, int n);
void Refaz(int Esq, int Dir, Entidade *A);
void Heapsort(Entidade *A, int n);

#endif