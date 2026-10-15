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

    bool right = true;
    bool up = false;
    bool left = true;
    bool down = false;

    int repete_operacao = 0;
    int cout = 0;

    printf("\n Digite um ponto da espiral quadrada e descubra suas coordenadas: ");
    scanf("%d", &pd);
    printf("\n Ponto escolhido: %d", pd);

    raiz_pd = sqrt(pd);
    repete_operacao = (int)raiz_pd;

    pa = pow((int)raiz_pd, 2);
    

    if ((int)raiz_pd == raiz_pd) {                        //verifica se o ponto desejado eh quadrado perfeito, tambem poderia usar: if ( ceil(raiz_pd) == raiz_pd ) {...}.  Se for quadrado perfeito, a partir de uma rapida analise da espiral, percebemos que faz parte da diagonal da espiral
        if ((int)raiz_pd % 2 == 0) {                     //verifica se o numero eh par, se sim, a partir de uma rapida analise da espiral, percebemos que esta no terceiro quadrante 
            cx = -(raiz_pd/2);
            cy = cx;
        } 
        else {                                            ////se o numero eh impar,a partir de uma rapida analise da espiral, percebemos que esta no primeiro quadrante
            cx = (raiz_pd-1)/2;
            cy = cx + 1;
        }
    }
    
    else if ((int)raiz_pd % 2 == 0) {                    //verifica se o numero eh par, se sim, o ponto desejado esta na parte inferior da diagonal formada pelos quadrados perfeitos
        cx = -((int)raiz_pd/2);
        cy = cx;
        
        for (pa; pa < pd; pa++) {     //percorre a espiral a partir do primeiro ponto quadrado perfeito anterior ao ponto desejado
        
            if (right) {
                cx ++;
                cout++;
            }
            else if (up) {
                cy ++;
            }

            if (cout == repete_operacao) {
                muda_direcao = true;
            }
            
            if (muda_direcao) {
                right = false;
                up = true;
                muda_direcao = false;
                cout = 0;
            }
        }
    }

    else {                                                           //se o numero eh impar, o ponto desejado esta na parte superior da diagonal formada pelos quadrados perfeitos
        cx = ((int)raiz_pd-1)/2;
        cy = cx + 1;

        for (pa; pa < pd; pa++) {             //percorre a espiral a partir do primeiro ponto quadrado perfeito anterior ao ponto desejado
        
            if (left) {
                cx --;
                cout++;
            }
            else if (down) {
                cy --;
            }

            if (cout == repete_operacao) {
                muda_direcao = true;
            }
            
            if (muda_direcao) {
                left = false;
                down = true;
                muda_direcao = false;
            }
        }
    }

    printf("\n Coordenadas: ( %d , %d )\n", cx, cy);

    return 0;
}