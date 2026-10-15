#include <stdio.h>

int main(int argc, char const *argv[])
{
    char string [128];
    char letra;
    int i = 0;
    printf("\nDigite uma liha: ");
    do{
        scanf("%c", &letra);
        if (letra >= 97 && letra <= 122)
        {
            letra -= 32;
        }
        string[i] = letra;
        i++;
    }while (letra != '\0' && letra != '\n');
    printf("\n Linha e maiusculo eh: %s", string);
    return 0;
}