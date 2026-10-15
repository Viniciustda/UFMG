#include <stdio.h>

int mmc(unsigned int num3, unsigned int num4) {
    int Mmc;
    if(num3 < num4) {
        Mmc =  num4;
    }else {
        Mmc = num3;
    }
    while(1) {
        if(Mmc % num3 == 0 && Mmc % num4 == 0)
            break;
        Mmc++;
    }
    return Mmc;
}

void main() {
    int num3;
    int num4;
    do {
        printf("\n Insira dois inteiros e vamos descobrir o MMC deles:");
        scanf("%d %d",&num3, &num4);
        fflush(stdin);
    } while(num3<=0 || num4<=0);
    printf("\n O MMC de %d e %d eh: %d\n", num3, num4, mmc(num3, num4));
}