#include <stdio.h>

int enesimoFibonacci(int U) {
    int cont = 2;
    int Fibonacci = 0;
    int auxU1 = 1;
    int auxU2 = 0;
    if(U == 1) {
        Fibonacci = 0;
    }else {
        do{
            auxU2 = auxU1 + Fibonacci;         
            Fibonacci = auxU1;                 
            auxU1 = auxU2;             
            cont ++;                            
        }while(cont <= U);
    }
    
    return Fibonacci;
}

void main() {
    
   int U;
    do { 
        printf("\n Digite um inteiro positivo e vamos retornar o enesimo termo da sequencia de Fibonacci:");
        scanf("%d", &U); 
        fflush(stdin);
    } while(U<=0);
    printf("\n O enesimo termo para %d eh: %d\n", U, enesimoFibonacci(U));
}