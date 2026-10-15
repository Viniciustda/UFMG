#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TAM_VETOR 1000
#define QTD_NUM 5000000

int main(int argc, char const *argv[]) {

    system("cls");

    int vetor_qtd[TAM_VETOR];
    int numero, maior, menor, soma;
    double media, desvio, somatorio = 0;

    for (int i = 0; i < TAM_VETOR; i++)
        vetor_qtd[i] = 0;
    for (int i = 0; i < QTD_NUM; i++) {
        do {
            numero = rand();
        } while (numero < 0 || numero > TAM_VETOR - 1);

        vetor_qtd[numero]++;
    }

    maior = vetor_qtd[0];
    menor = vetor_qtd[0];
    soma = vetor_qtd[0];

    for (int i = 1; i < TAM_VETOR; i++) {
        if (vetor_qtd[i] >= maior)
            maior = vetor_qtd[i];
        if (vetor_qtd[i] <= menor)
            menor = vetor_qtd[i];
        soma += vetor_qtd[i];
    }
    
    media = (double)soma / (double)TAM_VETOR;
    
    for (int i = 0; i < TAM_VETOR; i++)
        somatorio += pow((double)vetor_qtd[i] - media, 2);
    
    desvio = sqrt(somatorio / (double)TAM_VETOR);

    printf("\n Diferenca = %d\n", maior - menor);
    printf("\n Desvio padrao = %.8lf\n", desvio);

    return 0;
}