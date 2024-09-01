#include <stdio.h>

int main(int argc, char const *argv[])
{
    char string [128];
    char letra;
    int i = 0;
    int j, meio;
    int palin = 1;
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
    j = i - 2;
    meio = j/2;
    i = 0;
    while (j >= meio)
    {
        if (string[j] != string[i])
        {
            palin = 0;
            break;
        }
        j--;
        i++;
    }
    if (palin)
    {
        printf("\n Eh um palindromo.");
    }else {
        printf("\n Nao eh um palindromo.");
    }
    
    return 0;

}