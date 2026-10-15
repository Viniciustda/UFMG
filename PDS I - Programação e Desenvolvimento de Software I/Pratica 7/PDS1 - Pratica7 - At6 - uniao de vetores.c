#include <stdio.h>
#include <stdlib.h>

#define TAM_VETOR 60

int main(int argc, char const *argv[]) {

    system("cls");

    int X[TAM_VETOR], Y[TAM_VETOR], U[2*TAM_VETOR];
    int tam_atual = 0, numero = 0, achei = 0;

    for (int i = 0; i < TAM_VETOR; i++)
    {
        do
        {
            X[i] = rand();
        } while (X[i] < 0 || X[i] > 365);
        
        do
        {
            Y[i] = rand();
        } while (Y[i] < 0 || Y[i] > 365);
    }

    printf("\n");

    for (int i = 0; i < TAM_VETOR; i++)
    {
        printf(" %d\n", X[i]);
        printf(" %d\n", Y[i]);
    }

    printf("\n");
    

    for (int i = 0; i < TAM_VETOR; i++)
    {
        numero = X[i];
        achei = 0;
        for (int j = 0; j < tam_atual; j++)
        {
            if (numero == U[j])
            {
                achei = 1;
                break;
            }
            
        }
        if (!achei)
        {
            U[tam_atual] = numero;
            tam_atual++;
        }
        
        numero = Y[i];
        achei = 0;
        for (int j = 0; j < tam_atual; j++)
        {
            if (numero == U[j])
            {
                achei = 1;
                break;
            }
            
        }
        if (!achei)
        {
            U[tam_atual] = numero;
            tam_atual++;
        }
    }

    printf("\n Vetor Uniao\n");

    for (int i = 0; i < tam_atual; i++)
        printf(" U[%d} = %d\n", i, U[i]);

    return 0;
}