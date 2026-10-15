#include <stdio.h>

float areaCirculo(float raio) {
    float area;
    return area = 3.141592 * raio * raio;
}

void main() {
    float raio;
    printf("\n Insira o raio do circulo:");
    scanf("%f",&raio);
    printf("\n Um circulo de raio %.2f, tem a area igual a %.2f.\n", raio, areaCirculo(raio));
}