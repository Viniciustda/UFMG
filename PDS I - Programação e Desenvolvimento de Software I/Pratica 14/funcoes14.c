#include <stdio.h>
#include <math.h>
#include "funcoes14.h"

//------------A1------------
int soma_cubos(int n) {
    if(n==1)
        return 1;
    else     
        return pow(n, 3) + soma_cubos(n-1);
}

//------------A2------------
void imprime_naturais_crescente(int n) {
    if(n==0) {
        printf(" %d ", n);
        return;
    } else {
        imprime_naturais_crescente(n-1);
        printf("%d ", n);
        return;
    }
}

//------------A3------------
void imprime_naturais_decrescente(int n) {
    if(n==0) {
        printf(" %d", n);
        return;
    } else {
        printf(" %d", n);
        imprime_naturais_decrescente(n-1);
        return;
    }
}

//------------A4------------
int soma_elementos(int *v, int n) {
    if(n-1==0)
        return v[n-1];
    else
        return v[n-1] + soma_elementos(v, n-1);
}

//------------A5------------
float media_elementos(int *v,int i, int n) {
    if(i==n-1)
        return (float)v[i]/(float)n;
    else
        return (float)v[i]/(float)n + media_elementos(v, i+1, n);
}

//------------A6------------
int potencia(int x, int y) {
    if(y==0)
        return 1;
    else
        return x * potencia(x, y-1);
}

//------------A7------------
void reverso_string (char *s) {
    if(*s=='\0')   
        return;
    else {
        reverso_string(s+1);
        printf("%c", *s);
        return;
    }
    
}

//------------A8------------
int reverso_numero (int num, int *soma) {
    int resto = 0;
    if(num==0)
        return *soma;
    else {
        resto = num%10;
        *soma *= 10;
        *soma += resto;
        reverso_numero(num/10, soma);
        return *soma;
    }
    
}

//------------A9------------
void imprime_binario(int num) {
    int bit = 0;
    if(num<2)
        bit = num;
    else if(num % 2 == 0) {
        imprime_binario(num/2);
        bit = 0;
    } else {
        imprime_binario(num/2);
        bit = 1;
    }
    printf("%d", bit);
    
}

//------------A10------------
int menor_elemento(int *v, int n) {
    int atual = v[n-1];
    int menor;
    if(n-1==0)
        return v[n-1];
    else{
        menor = menor_elemento(v, n-1);
        if(menor <= atual)
            return menor;
        else
            return atual;
    }
    
}