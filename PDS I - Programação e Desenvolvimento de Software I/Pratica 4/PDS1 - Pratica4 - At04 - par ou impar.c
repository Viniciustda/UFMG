#include <stdio.h>

int ehPar(int n) {
    if(n % 2 == 0) {
        return 1;
    } 
    else { 
        return 0; 
    } 
}

void main() {
    int n;
    printf("\n Insira um inteiro e vamos descobrir se ele eh par ou impar:");
    scanf("%d", &n);
    printf("\n Seu numero eh %s.\n", ehPar(n) ? "par" : "impar");
}