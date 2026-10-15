#include <stdio.h>

int numeroDivisores(int g) {
    int div = 1;
    int ndiv = 0;
    do {
        if(g % div == 0) {
            ndiv ++;
            div ++;
        }else {
            div ++;
        }
    } while(div <= g);
    return ndiv;
}

void main() {

    int g;
    printf("\n Insira um inteiro e vamos descobrir quantos sao seus divisores:");
    scanf("%d",&g);
    fflush(stdin);
    printf("\n %d tem %d divisores.\n", g, numeroDivisores(g));

}