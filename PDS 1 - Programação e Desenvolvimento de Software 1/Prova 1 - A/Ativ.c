#include <stdio.h>
#include <stdlib.h>

/*
Sumário

Nesta prova, você vai implementar funções que compõem um jogo simples de adivinhação de números inteiros. 
Inicialmente, um usuário informa o número de tentativas (n) e o número inteiro (chave) a ser adivinhado. 
Em seguida, outro usuário entra com palpites para a chave. 
O jogo termina quando esse usuário acertar a chave ou quando o número de palpites alcançar n. 
Assuma que o usuário irá sempre entrar com dados do mesmo tipo que é pedido no exercício 
(ex: não entrará com um valor float quando é pedido um int), 
sem também extrapolar a quantidade que é pedida 
(ex: se é pedido dois valores inteiros, o usuário sempre entrará com dois valores inteiros). 

*/

// Exercicio 1

/*

1) Implemente uma função de nome init que não retorna nada (void) 
e que recebe dois endereços de memória de inteiros como parâmetros. 
No primeiro endereço, você deve armazenar a chave, que deverá ser lida do teclado. 
No segundo endereço, você deve armazenar o número de tentativas n, que também deverá ser lido do teclado. 
Antes de ler a chave, imprima “Chave:” na tela. Antes de ler n, imprima “Tentativas:” na tela. 
Se o valor de n digitado pelo usuário for menor ou igual a zero, troque o valor para 1. 
Antes de terminar a função, limpe a tela usando o comando system (o VPL roda em um sistema operacional Linux). 
Protótipo:

void init (int *chave, int *n);
*/

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

// Exercicio 2

/*
2) Implemente uma função de nome testa que recebe um palpite (inteiro) 
e uma chave (inteiro) como parâmetros e verifica se o palpite é igual à chave. 
Se forem iguais, imprima “Acertou!\n” e retorne 1. 
Se a diferença do palpite e da chave for de apenas 1, imprima “Errou por 1!\n” e retorne 0. 
Se a chave for maior, imprima “Maior!\n” e retorne 0. 
Se a chave for menor, imprima “Menor!\n” e retorne 0. 
Protótipo:

int testa(int palpite, int chave);
*/

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

// Exercicio 3

/*
3) Implemente uma função de nome jogo que recebe uma chave (inteiro) 
e o número de tentativas n (inteiro) como parâmetros. 
A sua função deve pedir palpites do usuário enquanto ele não acertar 
ou enquanto o número de tentativas não alcançar n. 
A cada palpite, você deve usar a função testa do exercício anterior para verificar se o usuário acertou. 
Se o usuário acertar o palpite dentro do limite de tentativas, a função retorna 1. 
Caso contrário, ela retorna 0. 
Protótipo:

int jogo(int chave, int n);
*/

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


// Exercicio 4

/* 
4) Implemente uma função de nome jogar que não recebe nenhum parâmetro e vai simular o seu procedimento main. 
Nesta função você deve chamar a função init para ler a chave e o número de tentativas n. 
Depois disso, você deve chamar a função jogo, enviando chave e n como parâmetros. 
Por fim, você deve imprimir a seguinte frase para o usuário: "Jogar novamente? (0=N, 1=S)\n" 
e ler um inteiro resposta dele. Se a resposta for 1, todos os passos acima, incluindo essa última pergunta, 
devem ser repetidos. Caso a resposta seja 0, a função termina. 
Protótipo:

void jogar();
*/

void jogar() {
    int resposta, chave, n;
    init(&chave, &n);
    jogo(chave, n);
    printf("Jogar novamente? (0=N, 1=S)\n");
    scanf("%d", &resposta);
    if(resposta == 1) {
        jogar();
    }else {
        exit(0);
    }
}

// Exercicio 5

/*
5) Altere a função jogo para permitir que o usuário obtenha uma dica sobre a chave quando o seu palpite é -999. 
Mais especificamente, quando o palpite do usuário for -999, 
você deve imprimir quantas unidades devem ser somadas ao seu palpite anterior para que ele acerte a chave. 
Exemplo: se a chave é 100, e o palpite anterior do usuário foi 5, 
então um palpite seguinte de -999 imprime 95. 
Importante #1: entrar com o palpite -999 não reduz o número de palpites restantes do usuário. 
Importante #2: se a chave é -999, então o jogo funciona como se esse exercício não existisse, ou seja, 
se o palpite for -999 quando a chave é -999, o programa se comporta como quando ele acerta uma chave qualquer. 
Importante #3: se o usuário digitar -999 antes de qualquer palpite, assuma que o seu primeiro palpite é 0 (zero).  
Resumo: -999 não deve ser considerado um palpite. -999 só é considerado um palpite quando a chave é -999.
*/