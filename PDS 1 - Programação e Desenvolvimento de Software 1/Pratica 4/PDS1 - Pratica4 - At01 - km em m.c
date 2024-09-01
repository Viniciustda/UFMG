#include <stdio.h>

float paraMetrosPorSegundo(float v) {
    return v /=3.6;
}

void main() {
    float v = 0;
    printf("\n Insira a velocidade em km/h:");
    scanf("%f",&v);
    printf("\n %.2f km/h eh igual a %.2f m/s.\n", v, paraMetrosPorSegundo(v));
}