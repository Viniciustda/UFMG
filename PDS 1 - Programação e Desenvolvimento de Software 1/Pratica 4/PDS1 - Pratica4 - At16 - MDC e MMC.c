#include <stdio.h>

int mdc(unsigned int num5, unsigned int num6) {
    int MdC = 0;
    unsigned int auxmdc = 1;
    do {
        if(num5 % auxmdc == 0 && num6 % auxmdc == 0) {
            MdC = auxmdc;
            auxmdc ++;
        }else {
            auxmdc ++;
        }
    } while(auxmdc <= num5 && auxmdc <= num6);
    return MdC;
}

int mmc(unsigned int num5, unsigned int num6) {
    int MmC;
    if(num5 < num6) {
        MmC =  num6;
    }else {
        MmC = num5;
    }
    while(1) {
        if(MmC % num5 == 0 && MmC % num6 == 0)
            break;
        MmC++;
    }
    return MmC;
}

void main() {
    int num5;
    int num6;
    do {
        printf("\n Insira dois inteiros e vamos descobrir o MDC e o MMC deles:");
        scanf("%d %d",&num5, &num6);
        fflush(stdin);
    } while(num5<=0 || num6<=0);
    printf("\n O MDC de %d e %d eh: %d", num5, num6, mdc(num5, num6));
    printf("\n E o MMC de %d e %d eh: %d\n", num5, num6, mmc(num5, num6));
}