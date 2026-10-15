#include "pratica4.h"
#include <stdio.h>



void main() {

//-----Teste atividade 1-----
    float v;
    printf("\n -----Teste atividade 1-----\n");
    printf("\n Insira a velocidade em km/h:");
    scanf("%f",&v);
    fflush(stdin);
    printf("\n %.2f km/h eh igual a %.2f m/s.\n", v, paraMetrosPorSegundo(v));

//-----Teste atividade 2-----
    float raio;
    printf("\n -----Teste atividade 2-----\n");
    printf("\n Insira o raio do circulo:");
    scanf("%f",&raio);
    fflush(stdin);
    printf("\n Um circulo de raio %.2f, tem a area igual a %.2f.\n", raio, areaCirculo(raio));

//-----Teste atividade 3-----
    int n1;
    int n2;
    int n3;
    printf("\n -----Teste atividade 3-----\n");
    printf("\n Insira 3 inteiros e vamos descobrir qual eh maior:");
    scanf("%d %d %d", &n1, &n2, &n3);
    fflush(stdin);
    printf("\n O maior inteiro inserido foi: %d\n", maior3(n1, n2, n3));

//-----Atividade 4-----
    int n;
    printf("\n -----Teste atividade 4-----\n");
    printf("\n Insira um inteiro e vamos descobrir se ele eh par ou impar:");
    scanf("%d", &n);
    fflush(stdin);
    printf("\n Seu numero eh %s.\n", ehPar(n) ? "par" : "impar");

//-----Atividade 5-----
    int N;
    printf("\n -----Teste atividade 5-----\n");
    printf("\n Insira um inteiro e vamos descobrir se ele eh divisivel por 3 ou 5, mas nao simultaneamente pelos dois, ou nao:");
    scanf("%d", &N);
    fflush(stdin);
    printf("\n Seu numero %s as expectativas.\n", ehDivisivelPor3ou5(N) ? "atende" : "nao atende");

//-----Atividade 6-----
    float h;
    char sexo;
    printf("\n -----Teste atividade 6-----\n");
    printf("\n Para calcularmos seu peso ideal, primeiro insira sua altura:");
    scanf("%f", &h);  
    fflush(stdin);
    printf("\n Agora, insira seu sexo (M ou F) e finalmente vamos descobrir seu peso ideal:");
    scanf("%c", &sexo);  
    fflush(stdin);
    printf("\n Seu peso ideal eh %0.3f.\n", pesoIdeal(h, sexo));

//-----Atividade 7-----
    float pa;
    float pi;
    printf("\n -----Teste atividade 7-----\n");
    printf("\n Agora vamos ver quanto voce precisa perder ou ganhar para atingir seu peso ideal. Quanto voce pesa atualmente?");
    scanf("%f", &pa);  
    fflush(stdin);
    pi = pesoIdeal(h, sexo);
    diferencaPeso(pa,pi);

//-----Atividade 8-----
    int m; 
    printf("\n -----Teste atividade 8-----\n");
    do { 
        printf("\n Insira um inteiro e vamos calcular a soma de todos os seus antecessores impares (caso seu inteiro tambem seja impar, ele sera incluido na soma):"); 
        scanf("%d", &m); 
    } while(m<=0); 
    printf("\n O resultado da soma eh: %d\n", somaImpares(m)); 

//-----Atividade 9-----
    int M;
    printf("\n -----Teste atividade 9-----\n");
    do { 
        printf("\n Insira um inteiro nao negativo e vamos calcular seu fatorial:"); 
        scanf("%d", &M); 
    } while(M<0); 
    printf("\n O fatorial de %d eh: %.0lf\n", M, fatorial(M));

//-----Atividade 10-----
    int b;
    printf("\n -----Teste atividade 10-----\n");
    printf("\n Insira um inteiro e vamos somar todos os seus antecessores divisiveis por 3 ou 5, mas nao simultaneamente pelos dois (caso seu numero atenda a condicao, ele sera incluido na soma):");
    scanf("%d", &b);
    fflush(stdin);
    printf("\n O resultado eh: %d\n", somaNumerosDiv3ou5(b));

//-----Atividade 11-----
    int x;
    int y;
    int z;
    int operacao;
    printf("\n -----Teste atividade 11-----\n");
    do { 
        printf("\n Insira tres inteiros maiores que zero e vamos calcular a media desejada:");
        scanf("%d %d %d", &x, &y, &z);
        fflush(stdin);
    } while(x<0 || y<0 || z<0);
    
    do { 
        printf("\n Agora escolha qual media voce quer calcular:\n (1 para geometrica)\n (2 para ponderada)\n (3 para harmonica)\n (4 para aritmetica)\n");
        scanf("%d", &operacao);
        fflush(stdin);
    } while(operacao<=0 || operacao>4);
    
    printf("\n O resultado da media escolhida entre %d, %d e %d eh: %.2f", x, y, z, calculaMedia(x, y, z, operacao));

//-----Atividade 12-----
    int g;
    printf("\n -----Teste atividade 12-----\n");
    printf("\n Insira um inteiro e vamos descobrir quantos sao seus divisores:");
    scanf("%d",&g);
    fflush(stdin);
    printf("\n %d tem %d divisores.\n", g, numeroDivisores(g));

//-----Atividade 13-----
    int U;
    printf("\n -----Teste atividade 13-----\n");
    do { 
        printf("\n Digite um inteiro positivo e vamos retornar o enesimo termo da sequencia de Fibonacci:");
        scanf("%d", &U); 
        fflush(stdin);
    } while(U<=0);
    printf("\n O enesimo termo para %d eh: %d\n", U, enesimoFibonacci(U));

//-----Atividade 14-----
    int num1;
    int num2;
    printf("\n -----Teste atividade 14-----\n");
    do {
        printf("\n Insira dois inteiros e vamos descobrir o MDC deles:");
        scanf("%d %d",&num1, &num2);
        fflush(stdin);
    } while(num1<=0 || num2<=0);
    printf("\n O MDC de %d e %d eh: %d\n", num1, num2, mdc(num1, num2));

//-----Atividade 15-----
    int num3;
    int num4;
    printf("\n -----Teste atividade 15-----\n");
    do {
        printf("\n Insira dois inteiros e vamos descobrir o MMC deles:");
        scanf("%d %d",&num3, &num4);
        fflush(stdin);
    } while(num3<=0 || num4<=0);
    printf("\n O MMC de %d e %d eh: %d\n", num3, num4, mmc(num3, num4));

//-----Atividade 16-----
    int num5;
    int num6;
    printf("\n -----Teste atividade 16-----\n");
    do {
        printf("\n Insira dois inteiros e vamos descobrir o MDC e o MMC deles:");
        scanf("%d %d",&num5, &num6);
        fflush(stdin);
    } while(num5<=0 || num6<=0);
    printf("\n O MDC de %d e %d eh: %d", num5, num6, MDC(num5, num6));
    printf("\n E o MMC de %d e %d eh: %d\n", num5, num6, MMC(num5, num6));
}