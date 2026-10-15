#include <stdio.h>

int main(int argc, char const *argv[])
{
    char linha[128];
    printf("\nDigite uma linha: ");
    fgets(linha, 128, stdin);
    int tamanho = 0;
    for (int i = 0; linha[i] != '\0' && linha[1] != '\n'; i++)
    {
        tamanho++;
    }
    printf("A linha possui %d caracteres.\n", tamanho);
    return 0;
}