#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define BUFFER_TAM 5

int main(int argc, char const *argv[]) {
        
    int contBuffer = 0, contTotal = 0;
    char buffer[BUFFER_TAM];
    char *texto = NULL;
    char *textoaux;
    char caractere;

    while (1) {    
        caractere = getche();

        if(caractere == '\r') {
            caractere = '\n';
            printf("\n");
        }

        if(contBuffer < BUFFER_TAM) {
            buffer[contBuffer] = caractere;
            contBuffer++;
        }

        if(contBuffer == BUFFER_TAM || caractere == '#') {
            textoaux = (char *) malloc((contTotal + contBuffer) * sizeof(char));
            
            for (int i = 0; i < contTotal; i++) 
                textoaux[i] = texto[i];
            for (int i = 0; i < contBuffer; i++)
                textoaux[contTotal + i] = buffer[i];

            if(texto != NULL)
                free(texto);

            texto = textoaux;
            contTotal += contBuffer;
            contBuffer = 0;
        }

        if (caractere == '#')
            break;
    } 

    texto[contTotal - 1] = '\0';
    printf("\n\n Texto digitado:\n %s", texto);
    free(texto);

    return 0;
}