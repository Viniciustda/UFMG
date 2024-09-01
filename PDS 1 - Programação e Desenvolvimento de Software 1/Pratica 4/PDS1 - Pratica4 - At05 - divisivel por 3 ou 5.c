#include <stdio.h>

int ehDivisivelPor3ou5(int N) {
    if(N % 3 == 0 || N % 5 == 0) {
        if(N % 3 != 0 || N % 5 != 0) {
            return 1;
        }
    } 
    else { 
        return 0; 
    } 
}

void main() {
    int N;
    printf("\n Insira um inteiro e vamos descobrir se ele eh divisivel por 3 ou 5, mas nao simultaneamente pelos dois, ou nao:");
    scanf("%d", &N);
    printf("\n Seu numero %s as expectativas.\n", ehDivisivelPor3ou5(N) ? "atende" : "nao atende");
}