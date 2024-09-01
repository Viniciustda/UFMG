#include <stdio.h>
#include <stdlib.h>


void init (int *chave, int *n) {
    printf("Chave:");
    scanf("%d", chave);
    printf("Tentativas:");
    scanf("%d", n);
    if (*n <= 0){
        *n = 1;
    }
    system("cls");                              //clear no linux e cls no windows
}

int testa(int palpite, int chave) {
    if(palpite == chave) {
        printf("Acertou!\n");
        return 1;
    }else if(palpite - chave == 1 || chave - palpite == 1) {
        printf("Errou por 1!\n");
        return 0;
    }else if(chave > palpite) {
        printf("Maior!\n");
        return 0;
    }else if(chave < palpite) {
        printf("Menor!\n");
        return 0;
    }   
}

int jogo(int chave, int n) {
    int palpite=0, palpiteanterior;
    
    while(n--){
        palpiteanterior = palpite;
        scanf("%d", &palpite);
        if(palpite == -999 && chave != -999) {
            printf("%d\n", chave - palpiteanterior);
            n++;
        }
        if(testa(palpite, chave)) {
            return 1;
        }
   
    }
    return 0;
}


void main() {
    int resposta, chave, n;
    init(&chave, &n);
    jogo(chave, n);
    printf("Jogar novamente? (0=N, 1=S)\n");
    scanf("%d", &resposta);
    if(resposta == 1) {
        main();
    }else {
        exit(0);
    }
}












/*
int jogo(int chave, int n) {
    int palpite;
    int tentativas =0;
    int n = 2;
    while ((testa(palpite,chave)==false) || (tentativas<=n)) {
        printf("Qual o seu palpite?\n");
        scanf("%d", &palpite);
        testa(palpite,chave);
        tentativas++;
    }
}
*/