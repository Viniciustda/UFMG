#include <stdio.h>

#define MAXTAM 100

void imprimeMatriz (float M[][MAXTAM], int n, int m) {
    int i=0, j=0;
    for ( i = 0; i < n; i++) {
        for ( j = 0; j < m; j++) {
            printf("%.1f ", M[i][j]);
        }
        printf("\n");
    }
    
}

int main() {

    float M1[MAXTAM][MAXTAM], M2[MAXTAM][MAXTAM];

    FILE *arq, *arqw;
    int n=0, m=0;
    int i=0, j=0;
    //opcoes de abertura do arquivo: r(read), w(write), a(adiciona linhas no final do arquivo)
    arq = fopen("matrizes_float.txt", "r");
    arqw = fopen("somaMatrizes.txt", "w");


    fscanf(arq, "%d %d", &n, &m);
    printf("\n %d %d", n, m);

    for ( i = 0; i < n; i++) {
        for ( j = 0; j < m; j++) {
            fscanf(arq, "%f", &M1[i][j]);
        }
    }
    printf("\n Matriz 1:\n");
    imprimeMatriz(M1,n,m);

    for ( i = 0; i < n; i++) {
        for ( j = 0; j < m; j++) {
            fscanf(arq, "%f", &M2[i][j]);
            M1[i][j] += M2[i][j];
        }
    }
    printf("\n Matriz 2:\n");
    imprimeMatriz(M2,n,m);

    printf("\n Soma matrizes:\n");
    imprimeMatriz(M1,n,m);

    for ( i = 0; i < n; i++) {
        for ( j = 0; j < m; j++) {
            fprintf(arqw, "%.1f ", M1[i][j]);
        }
        fprintf(arqw, "\n");
    }



    fclose(arq);
    fclose(arqw);

    return 0;
}