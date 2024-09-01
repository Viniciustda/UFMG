#include <stdio.h> 

int somaImpares(int m) { 

    int soma = 0, k =1; 

    while(k<=m) { 
        soma += k; 
        k += 2; 
    } 

    return soma; 
} 
 

void main() { 

    int m; 
    do { 
        printf("\n Insira um inteiro e vamos calcular a soma de todos os seus antecessores impares (caso seu inteiro tambem seja impar, ele sera incluido na soma):"); 
        scanf("%d", &m); 

    } while(m<=0); 
    printf("\n O resultado da soma eh: %d\n", somaImpares(m)); 
}