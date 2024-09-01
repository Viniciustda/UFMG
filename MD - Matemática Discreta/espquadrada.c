#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char const *argv[]) {

    int ponto_desejado = 0;  
    int ponto_base = 0;                  //ponto quadrado perfeito anterior ao ponto desejado

    int coordenada_x = 0;     
    int coordenada_y = 0;    

    int distancia_total = 0;            //distancia do ponto_base ao ponto_desejado
    int distancia_horizontal = 0;       //dentro da distancia_total, a distancia_horizontal do ponto_base ao ponto_desejado
    int distancia_vertical = 0;         //dentro da distancia_total, a distancia_vertical do ponto_base ao ponto_desejado

    float raiz_ponto_desejado = 0.0;   

    scanf("%d", &ponto_desejado);

    raiz_ponto_desejado = sqrt(ponto_desejado);

    ponto_base = pow((int)raiz_ponto_desejado, 2);

    distancia_total = ponto_desejado - ponto_base;
    
    if (distancia_total > (int)raiz_ponto_desejado) {                        //distancia_horizontal pode ser no maximo (int)raiz_ponto_desejado, entao se distancia_total > (int)raiz_ponto_desejado, distancia_vertical > 0
        distancia_vertical = distancia_total - (int)raiz_ponto_desejado;
        distancia_horizontal = distancia_total - distancia_vertical;
    } 
    else {                                                                   //distancia_horizontal pode ser no maximo (int)raiz_ponto_desejado, entao se distancia_total <= (int)raiz_ponto_desejado, distancia_vertical = 0
        distancia_horizontal =  distancia_total;
    }
    
    if ((int)raiz_ponto_desejado % 2 == 0) {                                //verifica se o numero eh par, se sim, o ponto desejado esta na parte inferior da diagonal formada pelos quadrados perfeitos
        coordenada_x = -((int)raiz_ponto_desejado/2);
        coordenada_y = coordenada_x;

        coordenada_x += distancia_horizontal;
        coordenada_y += distancia_vertical;
    }

    else {                                                                    //se o numero eh impar, o ponto desejado esta na parte superior da diagonal formada pelos quadrados perfeitos
        coordenada_x = ((int)raiz_ponto_desejado-1)/2;
        coordenada_y = coordenada_x + 1;

        coordenada_x -= distancia_horizontal;
        coordenada_y -= distancia_vertical;
    }

    printf("(%d,%d)", coordenada_x, coordenada_y);

    return 0;
}