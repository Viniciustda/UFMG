#include <stdio.h>
#include <stdlib.h>

#define TAM 10
#define N_OLHOS 3
#define LIM_GRITOS 3

int main() {
    
    //variaveis
    char sonho[TAM];
    int n_lot = 0;
    int gritos = 0;

    while (gritos < LIM_GRITOS) {

        //recebe a entrada
        //scanf("%[^\n]s", &sonho);
        fgets(sonho, TAM, stdin);
        printf("sonho:%s", sonho);

        //verifica se a entrada eh uma piscada ou um grito
        if (sonho[0] == 'c') {
            printf("nl:%d\n", n_lot);
            n_lot = 0;
            gritos ++;
            printf("gritos:%d \n", gritos);
        }
        else {
            for (int i = 0; i < N_OLHOS; i++) {
                if (sonho[i] == '*') {
                    if (i == 0) {
                        n_lot += 4;
                    }
                    else if (i == 1) {
                        n_lot += 2;
                    }
                    else {
                        n_lot ++;
                    }
                    
                }
                
            }
            
        } 
        printf("n lot:%d\n\n", n_lot);
    }

    return 0;
}

/*

1848

Como se sabe, existe um corvo com três olhos. O que não se sabia é que o corvo com três olhos pode prever o resultado da loteria de Westeros. Enquanto todos os outros corvos coletam as apostas, o corvo de três olhos já sabe o resultado, e quando Bran sonha com o corvo, o corvo conta o resultado. O problema é que Bran apesar de lembrar do sonho, não consegue interpretá-lo sozinho em tempo hábil. A sua tarefa é fazer um programa para interpretar o sonho de Bran e calcular o resultado da loteria.

Durante o sonho, o corvo pisca diversas vezes e grita apenas 3 vezes. A cada grito um número do resultado da loteria é calculado.

Cada piscada do corvo comunica um número em binário. Um olho aberto significa 1 e um olho fechado significa 0. O olho da esquerda é o mais significativo e o da direita é o menos significativo. A cada piscada, este número deve ser somado, e quando o corvo grita, essa soma é um resultado.

Entrada
A entrada descreve, em cada linha, em sequência, ou um grito ou uma piscada do corvo.

Um grito é representado pela string caw caw

Uma piscada é representada por três caracteres * ou -, representando, respectivamente, um olho aberto ou um olho fechado, da esquerda para a direita.

Lembre-se que o corvo tem 3 olhos.

Os números sorteados na loteria não excedem 1000.

Saída
A saída são três linhas, cada linha com um número da loteria.

*/