#include <stdio.h>

int EhDivisivelPor3ou5(int j) {
    if(j % 3 == 0 || j % 5 == 0) {
        if(j % 3 != 0 || j % 5 != 0) {
            return 1;
        }
    } else { 
        return 0; 
    } 
}

int somaNumerosDiv3ou5(int b) {
    int Soma = 0;
    int j = 1;
    while(j <= b) {
        if(EhDivisivelPor3ou5(j) == 1) {
            Soma += j;
            j ++;
        } else {
            j ++;
        }
    }

    return Soma;
    
}

void main() {
    int b;
    printf("\n Insira um inteiro e vamos somar todos os seus antecessores divisiveis por 3 ou 5, mas nao simultaneamente pelos dois (caso seu numero atenda a condicao, ele sera incluido na soma):");
    scanf("%d", &b);
    fflush(stdin);
    printf("\n O resultado eh: %d\n", somaNumerosDiv3ou5(b));
}