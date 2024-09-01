#include <stdio.h>
#include <math.h>

float calculaMedia(int x, int y, int z, int operacao) {

    float Media;
    if (operacao == 1) {
        Media = pow((x*y*x),(1.0/3.0));
        
    }else if (operacao == 2) {
        Media = ((x+2)*(y+3)*(z))/6.0;
        
    }else if (operacao == 3) {
        Media = 3/((1.0/x)+(1.0/y)+(1.0/z));
        
    }else if (operacao == 4) {
        Media = (x+y+z)/3.0;
        
    }
    return Media;
}

void main() {

    int x;
    int y;
    int z;
    int operacao;
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

}