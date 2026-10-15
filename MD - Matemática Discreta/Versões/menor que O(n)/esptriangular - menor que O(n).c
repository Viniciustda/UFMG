#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

int main(int argc, char const *argv[]) {

    int pd = 0;   //pd = ponto desejado
    int pa = 0;   //pa = ponto atual

    int cx = 0;     //cx = coordenada x
    int cy = 0;     //cy = coordenada y

    float raiz_pd = 0.0;    //raiz_pd = raiz do ponto desejado

    bool muda_direcao = false;

    bool up = true;
    bool down = false;

    int repete_operacao = 0;
    int cout = 0;

    printf("\n Digite um ponto da espiral triangular e descubra suas coordenadas: ");
    scanf("%d", &pd);
    printf("\n Ponto escolhido: %d", pd);

    raiz_pd = sqrt(pd);
    repete_operacao = (int)raiz_pd;

    pa = pow((int)raiz_pd, 2);

    if ((int)raiz_pd == raiz_pd) {                        //verifica se o ponto desejado eh quadrado perfeito, tambem poderia usar: if ( ceil(raiz_pd) == raiz_pd ) {...}.  Se for quadrado perfeito, a partir de uma rapida analise da espiral, percebemos que faz parte da diagonal da espiral
        if ((int)raiz_pd % 2 == 0) {                     //verifica se o numero eh par, se sim, a partir de uma rapida analise da espiral, percebemos que esta no terceiro quadrante 
            cx = -raiz_pd;
            cy = -(raiz_pd/2);
        } 
        else {                                            ////se o numero eh impar,a partir de uma rapida analise da espiral, percebemos que esta no quarto quadrante
            cx = raiz_pd;
            cy = -((raiz_pd-1)/2);
        }
    }
    
    else if ((int)raiz_pd % 2 == 0) {                    //verifica se o numero eh par
        cx = -(int)raiz_pd;
        cy = -((int)raiz_pd/2);
        
        for (pa; pa < pd; pa++) {     //percorre a espiral a partir do primeiro ponto quadrado perfeito anterior ao ponto desejado
            cx ++;
        }
    }

    else {                                                           //quando o numero eh impar
        cx = (int)raiz_pd;
        cy = -(((int)raiz_pd-1)/2);
        
        for (pa; pa < pd; pa++) {             //percorre a espiral a partir do primeiro ponto quadrado perfeito anterior ao ponto desejado
        
            if (up) {
                cx --;
                cy ++;
                cout++;
            }

            else if (down) {
                cx --;
                cy --;
            }

            if (cout == repete_operacao) {
                muda_direcao = true;
            }
            
            if (muda_direcao) {
                up = false;
                down = true;
                muda_direcao = false;
                cout = 0;
            }
        }
    }

    printf("\n Coordenadas: ( %d , %d )\n", cx, cy);

    return 0;
}