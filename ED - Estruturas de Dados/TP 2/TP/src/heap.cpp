#include "heap.hpp"
#include "entidade.hpp"
#include <iostream>
#include <string>

using namespace std;

void Constroi(Entidade *A, int n) {

    int Esq;

    Esq = float(n) / 2 + 0.5;

    while (Esq > 0) {
        Esq--;
        Refaz(Esq, n, A);
    }
}

void Refaz(int Esq, int Dir, Entidade *A) {

    int i, j;
    Entidade x;
    
    i = Esq;
    j = i * 2 + 1;
    x = A[i];

    while (j <= Dir) {

        if (j < Dir) {
            if ( A[j].GetVisitas() > A[j+1].GetVisitas() ) j++;
            else if ( A[j].GetVisitas() == A[j+1].GetVisitas() ) {
                if ( A[j].GetConteudo() < A[j+1].GetConteudo() ) 
                    j++; 
            }
        }

        if ( x.GetVisitas() < A[j].GetVisitas() ) break;
        if ( x.GetVisitas() == A[j].GetVisitas() ) {
            if ( x.GetConteudo() > A[j].GetConteudo() ) 
                break; 
        }

        A[i] = A[j];
        i = j;
        j = i * 2 + 1;
    }
    A[i] = x;
}

void Heapsort(Entidade *A, int n) {

    int Esq, Dir;
    Entidade x;
    
    Constroi(A, n); /* constroi o heap */

    Esq = 0; Dir = n;

    while (Dir > 0) { /* ordena o vetor */

        x = A[0];
        A[0] = A[Dir];
        A[Dir] = x;

        Dir--;

        Refaz(Esq, Dir, A);
    }
}