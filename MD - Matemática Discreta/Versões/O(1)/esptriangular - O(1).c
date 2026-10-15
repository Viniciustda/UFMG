#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char const *argv[]) {

    int pd = 0;   //pd = ponto desejado
    int pa = 0;   //pa = ponto atual (ponto quadrado perfeito anterior ao ponto desejado)

    int cx = 0;     //cx = coordenada x
    int cy = 0;     //cy = coordenada y

    int dt = 0;     //dt = distancia total (distancia do pa ao pd)
    int dh = 0;     //dh = distancia horizontal (dentro da dt, a dh do pa ao pd)
    int dv = 0;     //dv = distancia vertical  (dentro da dt, a dv do pa ao pd)

    float raiz_pd = 0.0;    //raiz_pd = raiz do ponto desejado

    printf("\n Digite um ponto da espiral triangular e descubra suas coordenadas: ");
    scanf("%d", &pd);
    printf("\n Ponto escolhido: %d", pd);

    raiz_pd = sqrt(pd);

    pa = pow((int)raiz_pd, 2);

    dt = pd - pa;
    dh = dt;
    if (dt > (int)raiz_pd) {                        //dv quando pd esta no 2 ou 3 quadrante
        dv = (int)raiz_pd - (dt - (int)raiz_pd);
    }                                               //dv quando pd esta no 1 ou 4 quadrante
    else {
        dv = dt;
    }

    if ((int)raiz_pd % 2 == 0) {                    //verifica se o numero eh par
        cx = -(int)raiz_pd;
        cy = -((int)raiz_pd/2);
        
        cx += dt;
    }

    else {                                                           //quando o numero eh impar
        cx = (int)raiz_pd;
        cy = -(((int)raiz_pd-1)/2);
       
        cx -= dh;
        cy += dv;
    }

    printf("\n Coordenadas: ( %d , %d )\n", cx, cy);

    return 0;
}