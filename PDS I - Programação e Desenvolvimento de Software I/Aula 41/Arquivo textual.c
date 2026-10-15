#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXTAM 100

int main() {

    int mat = 0;
    char *nome = NULL;
    float nota1 = 0, nota2 = 0;

    float soma = 0;
    int count_notas = 0;

    float maior_nota = -1;
    char nome_melhor[MAXTAM];

    FILE *arq = fopen("nota.dat", "r");                    // abre o arquivo
    FILE *arq_esp = fopen("Especial.dat", "w");            

    char linha[MAXTAM];

    while(!feof(arq)) {                                   // retorna se o arquivo acabou o não. se não acabou retorna 0, se acabou retorna 1.
        fgets(linha, MAXTAM, arq);                        // le a linha de um arquivo e armazena em um vetor de caracteres. neste caso armasena no vetor "linha", um total de "MAXTAM" caractesres, do arquivo "arq"
        //printf("\n %s\n", linha);
        //separar em strtok
        mat = atoi(strtok(linha, ","));    //atoi vai converter a string (ex.:3) em int (3) - pertence a stdlib.h
        nome = strtok(NULL, ",");
        nota1 = atof(strtok(NULL, ","));   //atof vai converter a string (ex.:3) em float (3) - pertence a stdlib.h
        nota2 = atof(strtok(NULL, ","));   //atof vai converter a string (ex.:3) em float (3) - pertence a stdlib.h

        printf("\n %d %s %.2f %.2f\n", mat, nome, nota1, nota2);

        if (nota1 + nota2 < 12) {
            fprintf(arq_esp, "%s, %.2f\n", nome, nota1+nota2);          // imprime no arquivo. neste caso imprime no arquivo "arq_esp"
        }
        
        if (nota1 + nota2 > maior_nota) {
            maior_nota = nota1 + nota2;
            strcpy(nome_melhor, nome);     //copia uma string de uma variavel pra outra, neste caso esta copiando a string de "nome" para "nome_melhor"
        }

        soma += nota1 + nota2;
        count_notas +=2;
    }

    printf("\n Media: %.2f\n", soma/count_notas);
    printf("\n Melhor aluno: %s (%.2f)\n", nome_melhor, maior_nota);

    fclose(arq);                              //fecha o arquivo
    fclose(arq_esp);

    return 0;
}