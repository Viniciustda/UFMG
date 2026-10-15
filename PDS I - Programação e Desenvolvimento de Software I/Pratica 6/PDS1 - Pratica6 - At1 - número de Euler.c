#include <stdio.h>
#include <math.h>

#define delta pow(10, -6)

double fatorial(int N) {
    double fat = 1;
    for (int i = 2; i <= N; i++)
    {
        fat *= i;
    }
    return fat;
}

int main(int argc, char const *argv[])
{
    double termo = 1;
    double euler = termo;
    for (int i = 1; termo > delta; i++)
    {
        termo = 1.0 / fatorial(i);
        euler += termo;
    }
    printf("\n O numero de Euler eh: %lf\n", euler);
    return 0;
}
