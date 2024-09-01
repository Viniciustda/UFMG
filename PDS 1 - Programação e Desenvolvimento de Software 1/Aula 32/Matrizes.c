/*
programa para simular a rolagem de dois dados. deve rolar dois dados 36.000 vezese :

imprimir quantas vezes cada arranjo ocorreu
imprimir a soma dos dados mais frequentes
imprimir o arranjo mais frequente
*/

#include <stdio.h>
#include <stdlib.h>

#define NFACES 6
#define NSOMAS (NFACES+NFACES-1)
#define NROLLS 36000

void zeraMatriz(int M[][NFACES], int n, int m) {

    for(int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            M[i][j] = 0;
        }
    } 
}

void imprimeMatriz(int M[][NFACES], int n, int m) {

    for(int i = 0; i < n; i++) {
        //imprimeVetor(M[i], m);
        for (int j = 0; j < m; j++) {
           printf("%5d ", M[i][j]);
        }
        printf("\n");
    } 
}

int rolaDado() {
    return 1 + rand()%NFACES;
}

void zeraVetor(int v[], int n) {

    for (int i = 0; i < n; i++) {
        v[i] = 0;
    }
}

void imprimeVetor(int v[], int n) {
  
    for (int i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

int main() {

    system("cls");
    
    int M[NFACES][NFACES];
    int somas[NSOMAS];
    int d1 = 0, d2 = 0, i = 0, j = 0;

    zeraMatriz(M, NFACES, NFACES);
    zeraVetor(somas, NSOMAS);

    for (i = 0; i < NROLLS; i++) {
        d1 = rolaDado();
        d2 = rolaDado();

        M[d1][d2]++; 
        somas[d1+d2]++;
    }
    
    imprimeMatriz(M, NFACES, NFACES);

    printf("\n Vetor de somas: ");
    imprimeVetor(somas, NSOMAS);

    int maior_i = 0;
    for(i = 1; i < NSOMAS; i++){
        if(somas[i] > somas[maior_i])
            maior_i = i;
    }
    printf("\n Soma mais frequente: %d\n", maior_i+2);

    int maior_j = 0;
    maior_i = 0;

    for ( i = 0; i < NFACES; i++) {
        for (j = 0; j < NFACES; j++) {
            if (M[i][j] > M[maior_i][maior_j]) {
                maior_i = i;
                maior_j = j;
            }
        }
    }
    
    printf("\n Arranjo mais frequente: %d, %d (%d)\n", maior_i+1, maior_j+1, M[maior_i][maior_j]);

    return 0;
}