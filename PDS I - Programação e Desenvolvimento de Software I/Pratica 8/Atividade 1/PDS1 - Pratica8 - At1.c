#include <stdio.h>
#include <stdlib.h>
#include "funcoes8.1.h"


int main(int argc, char const *argv[]) {
    system("cls");

    int matriz[TAM_MAX][TAM_MAX];
    int m, n, k;
    int zeros = 0, comp_zeros = 0;

    do {
        printf("\n Digite m e n, que representam as dimencoes da matriz (ate 100): ");
        scanf("%d %d", &m, &n);
        if (m < 0 || m > TAM_MAX || n < 0 || n > TAM_MAX) {
            printf("\n Dimencao invalida.\n");
        }
    } while (m < 0 || m > TAM_MAX || n < 0 || n > TAM_MAX);

    do {
        printf("\n Digite k (min 2): ");
        scanf("%d", &k);
        if (k < 2) {
            printf("\n Valor invalido.\n");
        }
    } while (k < 2);
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            do {
                matriz[i][j] = rand();
            } while (matriz[i][j] < 1 || matriz[i][j] > k);
        }
    }
    
    printf("\n Matriz preenchida\n");
    imprime_matriz(matriz, m, n);

    while (1)
    {
        comp_zeros = zeros;
        zeros = zera_3consec(matriz, m, n);
        if (comp_zeros == zeros)
            break;
        /*
        printf("\n Matriz zerada\n");
        imprime_matriz(matriz, m, n);
        */
        translada_zeros(matriz, m, n);
        /*
        printf("\n Matriz transladada\n");
        imprime_matriz(matriz, m, n);
        printf("\n A matriz possui %d zeros.\n", zeros);
        */
    }
    
    printf("\n Matriz final\n");
    imprime_matriz(matriz, m, n);
    printf("\n A matriz final tem %d zeros.\n", zeros);

    return 0;
}