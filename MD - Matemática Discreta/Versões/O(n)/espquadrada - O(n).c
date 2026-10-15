#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char const *argv[]) {
    
    bool muda_direcao = false;

    bool up = true;
    bool left = false;
    bool down = false;
    bool right = false;

    int repete_operacao = 1;
    int cout = 0;

    int ponto_desejado = 0;
    int coordenada_x = 0;
    int coordenada_y = 0;

    printf("\n Digite um ponto da espiral quadrada e descubra suas coordenadas: ");
    scanf("%d", &ponto_desejado);
    printf("\n Ponto escolhido: %d", ponto_desejado);

    for (int i = 0; i < ponto_desejado; i++) {
        
        
        if (up) {
            coordenada_y ++;
            cout++;
        }
        else if (left) {
            coordenada_x --;
            cout++;
        }
        else if (down) {
            coordenada_y --;
            cout++;
        }
        else if (right) {
            coordenada_x ++;
            cout++;
        }

        if (cout == repete_operacao) {
            muda_direcao = true;
        }
        
        if (muda_direcao) {
            if (up) {
                up = false;
                left = true;
            }
            else if (left) {
                left = false;
                down = true;
                repete_operacao++;
            }
            else if (down) {
                down = false;
                right = true;
            }
            else if (right) {
                right = false;
                up = true;
                repete_operacao++;
            }
            muda_direcao = false;
            cout = 0;
        }
    }

    printf("\n Coordenadas: ( %d , %d )\n", coordenada_x, coordenada_y);

    return 0;
}