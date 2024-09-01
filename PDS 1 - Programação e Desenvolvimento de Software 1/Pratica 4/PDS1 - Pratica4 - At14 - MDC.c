#include <stdio.h>

int mdc(unsigned int num1, unsigned int num2) {
    int Mdc = 0;
    unsigned int auxmdc = 1;
    do {
        if(num1 % auxmdc == 0 && num2 % auxmdc == 0) {
            Mdc = auxmdc;
            auxmdc ++;
        }else {
            auxmdc ++;
        }
    } while(auxmdc <= num1 && auxmdc <= num2);
    return Mdc;
}

void main() {
    int num1;
    int num2;
    do {
        printf("\n Insira dois inteiros e vamos descobrir o MDC deles:");
        scanf("%d %d",&num1, &num2);
        fflush(stdin);
    } while(num1<=0 || num2<=0);
    printf("\n O MDC de %d e %d eh: %d\n", num1, num2, mdc(num1, num2));
}