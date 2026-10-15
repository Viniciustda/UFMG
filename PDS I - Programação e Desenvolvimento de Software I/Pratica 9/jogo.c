#include <stdio.h>
#include <stdlib.h>
#include "jogo.h"

#define NFACES 6

int cont_id = 1;

int rolaDados() {
    int soma = 0;
    int dado;
    
    //printf("\n");
    for (int i = 0; i < 3; i++)
    {
        dado = 1+rand()%NFACES;
        //printf(" Valor do dado: %d\n", dado);
        soma += dado;
    }
    //printf("\n Soma dos dados eh %d\n", soma);
    return soma;
}

void criaGuerreiro(Guerreiro *x) {
    int vida=0;

    x->ataque = rolaDados();
    x->carisma = rolaDados();
    x->defesa = rolaDados();

    for (int i = 0; i < 3; i++)
        vida += rolaDados();
    x->pontos_vida = vida;
    
    x->id_guerreiro = cont_id;
    cont_id++;
}

int bonusCarisma(int carisma) {
    int bonus = 0;
    
    switch (carisma)
    {
        case 18:
            bonus = 3;
            break;
        case 16 ... 17:
            bonus = 2;
            break;
        case 14 ... 15:
            bonus = 1;
            break;
        case 6 ... 7:
            bonus = -1;
            break;
        case 4 ... 5:
            bonus = -2;
            break;
        case 3:
            bonus = -3;
            break;
        default:
            bonus = 0;
            break;
    }

    return bonus;
}

void ataca(Guerreiro *a, Guerreiro *b) {
    int dano, golpe, escudo;

    golpe = rolaDados() + a->ataque + bonusCarisma(a->carisma);
    escudo = rolaDados() + b->defesa + bonusCarisma(b->carisma);
    dano = golpe - escudo;

    //printf("\n Golpe: %d\n Escudo: %d\n Dano: %d", golpe, escudo, dano);
    /*printf("\n Bonus de carisma do %d: %d\n Bonus de carisma do %d: %d\n", 
        a->id_guerreiro, bonusCarisma(a->carisma), b->id_guerreiro, bonusCarisma(b->carisma));*/

    if (dano > 0) {
        b->pontos_vida -= dano;
        if (b->pontos_vida < 0)
            b->pontos_vida = 0;
    }
    
    //printf("\n Vida do defensor: %d\n", b->pontos_vida);

}

void jogo() {
    Guerreiro a, b;
    Guerreiro *aux, *atacante, *defensor;

    criaGuerreiro(&a);
    criaGuerreiro(&b);

    atacante = &a;
    defensor = &b;

    while (1)
    {
        ataca(atacante, defensor);
        if(defensor->pontos_vida <= 0)
            break;
        aux = atacante;
        atacante = defensor;
        defensor = aux;
    }
    
    printf("\nVencedor: %d", atacante->id_guerreiro);
    printf("\nPontos de vida restantes: %d\n", atacante->pontos_vida);
}