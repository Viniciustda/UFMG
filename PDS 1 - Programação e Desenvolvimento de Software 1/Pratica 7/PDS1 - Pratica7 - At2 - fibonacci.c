#include <stdio.h>

#define TAM_MAX 1000

int main(int argc, char const *argv[])
{

    int fib = 0;
    double seq[TAM_MAX];
    seq[0] = 1;
    seq[1] = 1;

    do
    {
        printf("\n Digite o valor de x tal que fib(x) representa a sequencia de fibonacci: ");
        scanf("%d", &fib);
        if (fib >= 0 && fib <= TAM_MAX)
        {
            for (int i = 2; i <= fib; i++)
            {
                seq[i] = seq[i-1] + seq[i-2];
            }
            printf("\n Fib(%d) = %4.3e\n", fib, seq[fib]);
        }
        else {
            printf("\n Fim da execucao.\n");
        }

    } while (fib >= 0 && fib <= TAM_MAX);
    

    return 0;
}