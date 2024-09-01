#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "3.h"

int main(int argc, char const *argv[])
{
    int a = 0;
    char str[] = "Bola";
    int n= 4;
    int M[4][4];

    M[0][0] = 0;
    M[0][1] = 1;
    M[0][2] = 0;
    M[0][3] = 1;
    M[1][0] = 1;
    M[1][1] = 0;
    M[1][2] = 0;
    M[1][3] = 1;
    M[2][0] = 0;
    M[2][1] = 0;
    M[2][2] = 0;
    M[2][3] = 1;
    M[3][0] = 1;
    M[3][1] = 1;
    M[3][2] = 1;
    M[3][3] = 0;
    
    //------------A1------------
    printf("\n-----Atividade 1-----\n");
    printf("\n Digite um numero para testar a funcao euler: ");
    scanf("%d", &a);
    printf(" Euler de %d: %.2f\n", a, euler(a));

    //------------A2------------
    printf("\n-----Atividade 2-----\n");
    printf("\n Digite um numero para testar a funcao serieDoisTercos: ");
    scanf("%d", &a);
    printf(" serieDoisTercos de %d: %.4f\n", a, serieDoisTercos(a));

    //------------A3------------
    
    printf("\n-----Atividade 3-----\n");
    printf("\n Ao converter a string em caixa alta, alteramos %d letras\n", caixaAlta(str));
    
    //------------A4------------
    printf("\n-----Atividade 6----\n");
    estatisticasNBA();
    return 0;
}
