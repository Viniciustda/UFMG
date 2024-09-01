#include <stdio.h>

int maior3(int n1, int n2, int n3) {
    if(n1 > n2 && n1 > n3) {
        return n1;
    } 
    else if(n2 > n1 && n2 > n3) { 
        return n2; 
    } 
    else if(n3 > n1 && n3 > n2) { 
        return n3; 
    }
}

void main() {
    int n1;
    int n2;
    int n3;
    printf("\n Insira 3 inteiros e vamos descobrir qual eh maior:");
    scanf("%d %d %d", &n1, &n2, &n3);
    printf("\n O maior inteiro inserido foi: %d\n", maior3(n1, n2, n3));
}