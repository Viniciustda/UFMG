#include <stdio.h>

// Letra (a)

float parteInteira(float x) {
    return (int)x;              //type casting
}

// Letra (b)

float parteFracionaria(float x) {
    return x - (int)x;              //type casting
}

// Letra (c)

float divInts(int x, int y) {
    return (float)x / y;              //type casting
}


// Teste

void main () {
    printf("\n   Letra (a)");
    printf("\n Entrada: -3.141592       Saida: %.0f", parteInteira(-3.141592));
    printf("\n Entrada: 0.15            Saida: %.0f", parteInteira(0.15));
    printf("\n Entrada: 12.06042019     Saida: %.0f", parteInteira(12.06042019));

    printf("\n\n   Letra (b)");
    printf("\n Entrada: -3.141592       Saida: %f", parteFracionaria(-3.141592));
    printf("\n Entrada: 255.001         Saida: %.3f", parteFracionaria(255.001));
    printf("\n Entrada: 12.0604         Saida: %.4f", parteFracionaria(12.0604));

    printf("\n\n   Letra (c)");
    printf("\n Entrada: -6 e 4           Saida: %.1f", divInts(-6, 4));
    printf("\n Entrada: 12 e 7          Saida: %.2f", divInts(12, 7));
    printf("\n Entrada: 9 e 26          Saida: %.3f\n", divInts(9, 26));
}