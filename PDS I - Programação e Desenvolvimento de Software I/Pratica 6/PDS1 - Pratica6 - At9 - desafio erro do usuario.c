#include <stdio.h>

int main(int argc, char const *argv[])
{
    char nome[128];
    char caractere;
    int pos = 0;
    int espaco = 1;
    printf("\n Digite seu nome completo (terminado em ponto final): ");
    do
    {
        caractere = getc(stdin);
        nome[pos] = caractere;
        pos++;
    } while (caractere != '.');
    pos = 0;
    while (nome[pos] != '.')
    {
        if (nome[pos] == ' ')
            espaco = 1;
        else if(espaco == 1) 
        {
            if (nome[pos] == 'd' || nome[pos] == 'D')
            {
                if (nome[pos+1] == 'e' || nome[pos+1] == 'E')
                {
                    if (nome[pos+2] == ' ')
                        pos += 1;
                }
                else if (nome[pos+1] == 'a' || nome[pos+1] == 'A')
                {
                    if (nome[pos+2] == ' ')
                        pos += 1;
                    else if (nome[pos+2] == 's' || nome[pos+2] == 'S')
                        if (nome[pos+3] == ' ')
                            pos += 2;
                    
                }
                else if (nome[pos+1] == 'o' || nome[pos+1] == 'O')
                {
                    if (nome[pos+2] == ' ')
                        pos += 1;
                    else if (nome[pos+2] == 's' || nome[pos+2] == 'S')
                        if (nome[pos+3] == ' ')
                            pos += 2;
                    
                }
            }
            else if(nome[pos] >= 97 && nome[pos] <= 122)
                printf("%c.", nome[pos] - 32);
            else
                printf("%c.", nome[pos]);
            espaco = 0;
        }
        pos++;
    }
    
    printf("\n");
    return 0;
}