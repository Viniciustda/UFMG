#include <stdio.h>
#include <stdlib.h>

#define TAM 1004

int main() {
    
    char teste[TAM];
    int n_teste = 0;
    int direita = 0;
    int esquerda = 0;

    scanf("%d",&n_teste);
    getchar();
    
    while (n_teste--) {
        fgets(teste, TAM, stdin);
        for (int i = 0; i < TAM; i++) {
            if (teste[i] == '<')
                esquerda++;
            else if (teste[i] == '>' && esquerda > direita)
                direita++;     
        }
        if (esquerda < direita)
            printf("%d\n",esquerda);
        else
            printf("%d\n",direita);
        esquerda = 0;
        direita = 0;
        for (int i = 0; i < TAM; i++)
            teste[i] = '\0';
    }
    

    return 0;
}

/*

1069

João está trabalhando em uma mina, tentando retirar o máximo que consegue de diamantes "<>". Ele deve excluir todas as particulas de areia "." do processo e a cada retirada de diamante, novos diamantes poderão se formar. Se ele tem como uma entrada .<...<<..>>....>....>>>., três diamantes são formados. O primeiro é retirado de <..>, resultando  .<...<>....>....>>>. Em seguida o segundo diamante é retirado, restando .<.......>....>>>. O terceiro diamante é então retirado, restando no final .....>>>., sem possibilidade de extração de novo diamante.

Entrada
Deve ser lido um valor inteiro N que representa a quantidade de casos de teste. Cada linha a seguir é um caso de teste que contém até 1000 caracteres, incluindo "<,>, ."

Saída
Você deve imprimir a quantidade de diamantes possíveis de serem extraídos em cada caso de entrada.

*/