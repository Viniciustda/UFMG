#include <stdio.h>
#include <stdlib.h>

#define TAM_MAX 100

int main(int argc, char const *argv[])
{
    
    double u[TAM_MAX], v[TAM_MAX];
    double produto;
    unsigned int n;
    system("cls");
    do
    {
        printf("\n Digite a dimensao dos vetores (ate 100): ");
        scanf("%d", &n);
        if (n > TAM_MAX)
            printf("\n Dimensao invalido.\n");
    } while (n > TAM_MAX);
    printf("\n Digite os valores de u entre espacos (Ex.: 1 2 3 ... 100): ");
    for (int i = 0; i < n; i++)
        scanf("%lf", &u[i]);
    printf("\n Digite os valores de v entre espacos (Ex.: 1 2 3 ... 100): ");
    for (int i = 0; i < n; i++)
        scanf("%lf", &v[i]);
    for (int i = 0; i < n; i++)
        produto += (u[i]*v[i]);
    printf("\n O produto escalar entre u e v eh %.2lf\n", produto);

    return 0;
}