#include <stdio.h>
#include <string.h>

// vou separar Vinicius Trindadee Dias Abel em 4 strings

int main(){

    char str[] = "Vinicius Trindade Dias Abel";
    char separador[] = " "; // posso definir quantos sepradores eu quiser
    char *token;

    /*
    printf("\n str: %s\n", str);
    token = strtok(str, separador);
    printf("\n str: %s\n", str);
    printf("\n token: %s\n", token);

    token = strtok(NULL, separador);
    printf("\n str: %s\n", str);
    printf("\n token: %s\n", token);
    */

   token = strtok(str, separador);

   while (token != NULL) {
        printf("\n token: %s\n", token);
        token = strtok(NULL, separador);
   }

    return 0;
}