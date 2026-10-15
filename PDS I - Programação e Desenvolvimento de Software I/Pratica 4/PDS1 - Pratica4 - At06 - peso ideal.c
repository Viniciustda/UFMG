#include <stdio.h>

float pesoIdeal(float h, char sexo) {
    float pi;
    if(sexo == 'M') {
        return pi = (72.7 * h) - 58;
    }
    else {
        return pi = (62.1 * h) - 44.7;
    }
}

void main() {
    float h;
    char sexo;
    printf("\n Para calcularmos seu peso ideal, primeiro insira sua altura:");
    scanf("%f", &h);  
    fflush(stdin);
    printf("\n Agora, insira seu sexo (M ou F) e finalmente vamos descobrir seu peso ideal:");
    scanf("%c", &sexo);  
    printf("\n Seu peso ideal eh %0.3f.\n", pesoIdeal(h, sexo));
}