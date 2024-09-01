#include <stdio.h>

double fatorial(int M) {
    double fatorial = 1;
    int i = 1; 
    if(M == 0) {
        return fatorial; 
    } 
    else {
        do { 
            fatorial *= i; 
            i ++; 
        } while(i<=M);
        return fatorial;
    }
    
}

void main() {
    int M;

    do { 
        printf("\n Insira um inteiro nao negativo e vamos calcular seu fatorial:"); 
        scanf("%d", &M); 
    } while(M<0); 
    printf("\n O fatorial de %.0lf eh: %d\n", M, fatorial(M)); 
   }