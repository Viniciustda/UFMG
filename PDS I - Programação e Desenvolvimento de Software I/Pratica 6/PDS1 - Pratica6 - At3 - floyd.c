#include <stdio.h>

int main(int argc, char const *argv[])
{
    int n, acum = 1;
    printf("\n Digite o numero de linhas que deseja que apareca no triangulo de Floyd(inteiro positivo): ");
    scanf("%d", &n);

    printf("\n A linha %d do triangulo de Floyd eh:\n\n", n);
    int inicial = (((n - 1) * n) / 2) + 1;
    for (int i = 1; i <= n; i++)
    {
        printf("%d ", inicial);
        inicial ++;
    }
    printf("\n");

    printf("\n Triangulo de Floyd completo:\n\n");
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= i; j++)
        {
            printf("%d ", acum);
            acum ++;
        }
        printf("\n");
    }
    return 0;
}