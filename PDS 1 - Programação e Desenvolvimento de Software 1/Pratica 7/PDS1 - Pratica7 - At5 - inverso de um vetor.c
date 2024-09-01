#include <stdio.h>
#include <stdlib.h>

#define TAM_MAX 1000

int main(int argc, char const *argv[]) {
    
    system("cls");

    int X[TAM_MAX], Y[TAM_MAX];
    int n;

    do {
       printf("\n Digte o tamanho do vetor (ate 1000): ");
       scanf("%d", &n);

       if (n < 0 || n > TAM_MAX)
           printf("\n Tamanho invalido.\n");
    } while (n < 0 || n > TAM_MAX);

    printf("\n Digite os %d elementos do vetor entre espacos (Ex.: 1 2 3 ... 1000): ");

    for (int i = 0; i < n; i++)
        scanf("%d", &X[i]);
    for (int i = 0; i < n; i++)
        Y[i] = X[n- 1 -i];
    for (int i = 0; i < n; i++)
        printf("\n Y[%d] = %d\n", i, Y[i]);
    
    return 0;
}
