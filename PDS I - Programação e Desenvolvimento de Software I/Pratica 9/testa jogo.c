#include <stdio.h>
#include "jogo.h"

int main(int argc, char const *argv[])
{
    jogo();
    /*

    Guerreiro a, b;
    Guerreiro *aux, *atacante, *defensor;

    criaGuerreiro(&a);

    printf("\n Informacoes do guerreiro a\n");
    printf("\n Id: %d\n", a.id_guerreiro);
    printf(" Ataque: %d\n", a.ataque);
    printf(" Carisma: %d\n", a.carisma);
    printf(" Defesa: %d\n", a.defesa);
    printf(" Vida: %d\n", a.pontos_vida);

    //----------------------------------------------------------

    criaGuerreiro(&b);

    printf("\n Informacoes do guerreiro b\n");
    printf("\n Id: %d\n", b.id_guerreiro);
    printf(" Ataque: %d\n", b.ataque);
    printf(" Carisma: %d\n", b.carisma);
    printf(" Defesa: %d\n", b.defesa);
    printf(" Vida: %d\n", b.pontos_vida);

    //----------------------------------------------------------

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

    */

    return 0;
}