#include <stdio.h>
#include <stdlib.h>
#include "selection.h"

int num_iguais (int aposta[], int cartela[], int n) {
    int igual = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (aposta[i] == cartela[j]) {
                igual++;
                break;
            }
        }
    }
    return igual;
}

int main () {

    FILE *arq = fopen("cartelas.txt", "r");

    int aposta[6];
    int num_sort[] = {6, 9, 22, 23, 48, 52};
    int num_lost[] = {4, 8, 15, 16, 23, 42};
    int num_cart_venc = 0;
    int num_cart_lost = 0;
    int num_cart_quadra = 0;
    int num_cart_quina = 0;
    int igual = 0;


    while(!feof(arq)) {
        
        igual = 0;

        for (int i = 0; i < 6; i++)
            fscanf(arq, "%d", &aposta[i]);

        ordenar_por_selecao(aposta, 6);

        igual = num_iguais(aposta, num_sort, 6);

        if (igual == 6)
            num_cart_venc++;

        if (igual == 5)
            num_cart_quina++;

        if (igual == 4)
            num_cart_quadra++;

        igual = num_iguais(aposta, num_lost, 6);

        if (igual == 6)
            num_cart_lost++;
        
    }
    
    printf("\n Houve um total de %d cartela(s() vencedora(s)!\n", num_cart_venc);
    printf("\n %d pessoa(s) conseguiu/conseguiram a quina!\n", num_cart_quina);
    printf("\n %d pessoa(s) conseguiu/conseguiram a quadra!\n", num_cart_quadra);
    printf("\n %d pessoa(s) apostou/apostaram nos numeros do seriado Lost! \n", num_cart_lost);

    fclose(arq);

    return 0;
}