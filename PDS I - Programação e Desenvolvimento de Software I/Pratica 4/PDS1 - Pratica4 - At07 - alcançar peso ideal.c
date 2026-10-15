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

float diferencaPeso(float pa, float pi){
    if(pa - pi == 0 ) {
        return printf("\n Voce ja esta com o peso ideal.\n");
    }else if(pa - pi < 0 ) {
        return printf("\n Voce precisa ganhar %0.3f kg para alcancar seu peso ideal.\n", (pa - pi)*-1);
    }else if(pa - pi > 0 ) {
        return printf("\n Voce precisa perder %0.3f kg para alcancar seu peso ideal.\n", pa - pi);
    }
}


void main() {
    float h;
    char sexo;
    float pa;
    float pi;
    printf("\n Para calcularmos seu peso ideal, primeiro insira sua altura:");
    scanf("%f", &h);  
    fflush(stdin);
    printf("\n Agora, insira seu sexo (M ou F) e finalmente vamos descobrir seu peso ideal:");
    scanf("%c", &sexo);  
    printf("\n Seu peso ideal eh %0.3f.\n", pesoIdeal(h, sexo));
    printf("\n Agora vamos ver quanto voce precisa perder ou ganhar para atingir seu peso ideal. Quanto voce pesa atualmente?");
    scanf("%f", &pa);  
    fflush(stdin);
    pi = pesoIdeal(h, sexo);
    diferencaPeso(pa,pi);
}