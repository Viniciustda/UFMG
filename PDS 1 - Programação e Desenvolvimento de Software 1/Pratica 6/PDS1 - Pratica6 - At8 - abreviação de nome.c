#include <stdio.h>

int main(int argc, char const *argv[])
{
    char caractere;
    printf("\n Digite seu nome completo (terminado em ponto final): ");
    do
    {
        caractere = getc(stdin);
        if (caractere >= 65 && caractere <= 90)
        {
            printf("%c.", caractere);
        }
    } while (caractere != '.');
    printf("\n");
    return 0;
}