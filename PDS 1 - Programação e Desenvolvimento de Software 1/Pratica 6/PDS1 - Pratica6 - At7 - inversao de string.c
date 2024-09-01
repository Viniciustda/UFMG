#include <stdio.h>

int main(int argc, char const *argv[])
{
    char original[128];
    char inversa[128];
    char c;
    int i = 0, j;
    printf("\n Digite uma string (terminada em ponto final): ");
    do
    {
        c = getc(stdin);
        original[i] = c;
        i++;
    } while (c != '.');
    j = i - 1;
    inversa[i] = '\0';
    i = 0;
    while (j >= 0)
    {
        inversa[j] = original[i];
        j--;
        i++;
    }
    printf("\n O inverso da string eh: %s\n", inversa);

    return 0;
}