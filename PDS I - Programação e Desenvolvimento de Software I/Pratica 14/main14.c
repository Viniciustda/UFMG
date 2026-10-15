#include <stdio.h>
#include "funcoes14.h"

int main(int argc, char const *argv[]) {

    int a = 0;
    int b[1] = {4};
    int c[2] = {3, 2};
    int d[10] = {9, 8, 2, 5, 4, 6, 3, 7, 1, 0};
    int e = 0;
    int f = 0;
    char *s1;
    char s2[6] = "Laika";
    int resultado = 0;
    int g = 0;
    int h = 0;

    //------------A1------------
    printf("\n-----Atividade 1-----\n");
    printf("\n Digite um numero para testar a funcao soma de cubos: ");
    scanf("%d", &a);
    printf(" Soma de cubos ate %d: %d\n", a, soma_cubos(a));

    //------------A2------------
    printf("\n-----Atividade 2-----\n");
    printf("\n Digite um numero natural para imprimir de 0 ate ele: ");
    scanf("%d", &a);
    printf(" Impressao de 0 ate %d:\n", a);
    imprime_naturais_crescente(a);
    //------------A3------------
    printf("\n\n-----Atividade 3-----\n");
    printf("\n Digite um numero natural para imprimir dele ate 0: ");
    scanf("%d", &a);
    printf(" Impressao de %d ate 0:\n", a);
    imprime_naturais_decrescente(a);

    //------------A4------------
    printf("\n\n-----Atividade 4-----\n");
    printf("\n Soma dos elementos do vetor b: %d\n", soma_elementos(b, 1));
    printf(" Soma dos elementos do vetor c: %d\n", soma_elementos(c, 2));
    printf(" Soma dos elementos do vetor d: %d\n", soma_elementos(d, 10));

    //------------A5------------
    printf("\n-----Atividade 5-----\n");
    printf("\n Media dos elementos do vetor b: %.2f\n", media_elementos(b, 0, 1));
    printf(" Media dos elementos do vetor c: %.2f\n", media_elementos(c, 0, 2));
    printf(" Media dos elementos do vetor d: %.2f\n", media_elementos(d, 0, 10));

    //------------A6------------
    printf("\n-----Atividade 6-----\n");
    printf("\n Digite uma base e um expoente: ");
    printf("\n Base: ");
    scanf("%d", &e);
    printf(" Expoente: ");
    scanf("%d", &f);
    printf(" Potencia de %d elevado a %d = %d\n", e, f, potencia(e, f));

    //------------A7------------
    printf("\n-----Atividade 7-----\n");
    printf("\n Reverso da string 'Vinicius': ");
    reverso_string("Vinicius");
    s1 = "Trindade";
    printf("\n Reverso da string '%s': ", s1);
    reverso_string(s1);
    printf("\n Reverso da string '%s': ", s2);
    reverso_string(s2);

    //------------A8------------
    printf("\n-----Atividade 8-----\n");
    printf("\n Digite um numero para ser invertido: ");
    scanf("%d", &g);
    printf(" O reverso do numero %d eh: %d\n", g, reverso_numero(g, &resultado));

    //------------A9------------
    printf("\n-----Atividade 9-----\n");
    printf("\n Digite um numero para ser impresso em binario: ");
    scanf("%d", &h);
    printf(" O numero %d em binario eh: ", h);
    imprime_binario(h);

    //------------A10------------
    printf("\n\n-----Atividade 10-----\n");
    printf("\n Menor elemento do vetor b: %d\n", menor_elemento(b, 1));
    printf(" Menor elemento do vetor c: %d\n", menor_elemento(c, 2));
    printf(" Menor elemento do vetor d: %d\n", menor_elemento(d, 10));
    
    return 0;
}