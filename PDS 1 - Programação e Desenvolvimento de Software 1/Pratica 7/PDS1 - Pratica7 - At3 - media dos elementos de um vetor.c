#include <stdio.h>
#include <stdlib.h>

#define TAM_MAX 1000

double mediaVetor(int vetor[], int n) {

    int soma = 0;
    for (int i = 0; i < n; i++) {
        soma += vetor[i];
    }
    return (double)soma / (double)n;
}

void preencheVetor(int *vetor, int n) {
    for (int i = 0; i < n; i++) {
        vetor[i] = rand();
    }
}

int main(int argc, char const *argv[]) {

    system("cls");

    int v[TAM_MAX];
    double media;
    int n;

    do {
       printf("\n Digte o tamanho do vetor (ate 1000): ");
       scanf("%d", &n);

       if (n < 0 || n > TAM_MAX)
           printf("\n Tamanho invalido.\n");
    } while (n < 0 || n > TAM_MAX);

    preencheVetor(v, n);
    media = mediaVetor(v, n);

    printf("\n A media eh %.2lf\n", media);
    
    return 0;
}

