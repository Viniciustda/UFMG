#include "quicksort.hpp"
#include "entidade.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void VerificaOrdemAlfabetica (Entidade *A, int tam) {
    Entidade aux;
    for (int i = 0; i < tam-1; i++) {
        if (A[i].GetVisitas() == A[i+1].GetVisitas()) {
            if (A[i].GetConteudo() > A[i+1].GetConteudo()) {
                aux.Preenche(A[i].GetConteudo(), A[i].GetRodada());
                A[i].Preenche(A[i+1].GetConteudo(), A[i+1].GetRodada());
                A[i+1].Preenche(aux.GetConteudo(), aux.GetRodada());
            }
        }
    }
    
}

int MedianaDeTres(int a, int b, int c) {
    if ((a > b) != (a > c)) 
        return a;
    else if ((b > a) != (b > c)) 
        return b;
    else
        return c;
}

void Particao(int Esq, int Dir, int *i, int *j, Entidade *A) {
    Entidade x;
    Entidade w;
    string prioridade;
    int meio;
    *i = Esq; 
    *j = Dir;
    meio = (*i + *j)/2;
    x = A[MedianaDeTres(Esq, meio, Dir)];    // obtem o pivo x 
    
    do {        
        while (x.GetVisitas() < A[*i].GetVisitas()) (*i)++;
        while (x.GetVisitas() > A[*j].GetVisitas()) (*j)--;
        if (*i <= *j) {
            w = A[*i]; 
            A[*i] = A[*j]; 
            A[*j] = w;
            (*i)++; (*j)--;
        }
    } while (*i <= *j);
}

void Ordena(int Esq, int Dir, Entidade *A) {
    int i;
    int j;
    Particao(Esq, Dir, &i, &j, A);
    if (Esq < j) Ordena(Esq, j, A);
    if (i < Dir) Ordena(i, Dir, A);
}

void QuickSort(Entidade *A, int n) {
    Ordena(0, n-1, A);
    VerificaOrdemAlfabetica(A, n);
}