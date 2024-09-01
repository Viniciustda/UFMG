#include <stdio.h>
#include <stdlib.h>

void aloca_matriz (float ***mat, int n, float inicial) {
    *mat = (float **) malloc(n * sizeof(float *));

    for (int i = 0; i < n; i++) {
        (*mat)[i] = (float *) malloc(n * sizeof(float));
        for (int j = 0; j < n; j++) {
            (*mat)[i][j] = inicial;
        }
    }
}

void libera_matriz (float ***mat, int n) {
    for (int i = 0; i < n; i++)
        free((*mat)[i]);
    free(*mat);
}

void imprime_matriz (float ***mat, int n) {
    printf("\n Matriz zerada\n");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            printf(" %.1f", (*mat)[i][j]);
        printf("\n");
    }
}

int main(int argc, char const *argv[]) {
    
    float **matriz;
    float inicial = 0.0;
    int n;

    printf("\n Digite a dimencao da matriz quadrada: ");
    scanf("%d", &n);

    aloca_matriz(&matriz, n, inicial);

    imprime_matriz(&matriz, n);

    libera_matriz(&matriz, n);
    
    return 0;
}