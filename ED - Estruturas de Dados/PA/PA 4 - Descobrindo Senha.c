#include <stdio.h>
#include <stdlib.h>

#define TAM 10

typedef struct Tecla { 
    float oleo; 
    int num; 
} Tecla; 

int main(int argc, char const *argv[]) {
    
    int n = 0;
    int caso = 1;
    float o = 0.0;
    Tecla v[TAM];

    int i = 0;
    int j = 0;
    Tecla aux;

    while (scanf("%d", &n) != EOF) {

        for ( i = 0; i < TAM; i++) {
            scanf("%f",&o);
            v[i].oleo = o;
            v[i].num = i;
        }
        
        for(i = 1; i < TAM; i++) {
            aux = v[i];
            j = i - 1;
            while(j >= 0 && v[j].oleo < aux.oleo) {
                v[j+1] = v[j];
                j--;
            }
            v[j+1] = aux;
        }

        printf("Caso %d: ", caso++);
        for ( i = 0; i < n; i++ )
            printf("%d", v[i].num);
        printf("\n");

    }

    return 0;
}

/*

2252

Sherlock é um garoto muito inteligente, que nasceu em Londres na Inglaterra. Desde os dois anos ele sempre gostou de solucionar desafios. Com três anos foi campeão escolar de Sudoku. Seu tio Paul Harrison sempre gostou de desafiar o garoto trazendo problemas de lógica. Dessa vez Paul lançou um desafio, mas não é só um desafio, ele quer descobrir a senha da sala de seu chefe, para conseguir pegar alguns documentos importantes da empresa. Sua tarefa é construir um programa que ajude o seu tio com esse problema, dado a informação da quantidade de óleo sobre cada tecla.

Por exemplo, 0 = 0.80, 1 = 0.12, 2 = 0.01, 3 = 0.22, 4 = 0.35, 5 = 0.25, 6 = 0.77, 7 = 0.50, 8 = 0.63, 9 = 0.45 e n = 5, a senha será 06879.

Entrada
A entrada é composta por diversos casos de teste, e é concluida com final de arquivo. A primeira linha de cada caso é composta por um valor inteiro N (0 < N < 11), que corresponde a quantidade de dígitos da senha. Em seguida lê-se 10 valores Vi, onde (0 <= Vi < 1), o i-ésimo valor corresponde ao nível de oleosidade da tecla i da fechadura. Quanto maior o nível de oleosidade de uma tecla mais vezes ela foi utilizada. A senha formada não contém dígitos repetidos e é formada pelos N dígitos mais ultilizados.

Saída
Para cada caso de teste imprima um linha com a mensagem "Caso k: ", onde k indica o número do caso de teste seguido pela senha de acesso a sala do chefe. Mostre o resultado conforme o exemplo de saída. Em caso de duas teclas terem o mesmo nível de oleosidade, a tecla com o menor valor deve preceder as outras. Imprimir a senha de forma que comece mostrando a tecla de maior oleosidade para a menor.

*/