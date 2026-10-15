//Não remova ou modifique os includes e defines abaixo!

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_DIAS 14
#define TAM_NOME 4
#define FRAC_VAR 0.1

#define ACAO_INI_MIN 5
#define ACAO_INI_MAX 100

#define MAX_ACOES 1000

/*
Complete o novo tipo de dados Acao que está definido na versão inicial da sua prova. 
Uma Acao contém os seguintes campos: 
a) um vetor de pontos flutuantes precos, que armazena a variação diária do preço da ação; 
b) um ponto flutuante max, que armazena o maior valor que a ação alcançou no período; 
c) um ponto flutuante min, que armazena o menor valor que a ação alcançou no período; 
d) uma string nome, que armazena o nome da ação. 
Considere que o vetor precos armazena o preço da ação por 14 dias (usar o #define NUM_DIAS) e 
o nome da ação tem 4 (usar o #define TAM_NOME) caracteres (ex: “DCC1”). 
*/

typedef struct Acao {
	//preencha os campos abaixo:
    float precos[NUM_DIAS];
    float max;
    float min;
    char nome[TAM_NOME+1];

	
} Acao;

//Não modifique ou apague o protótipo abaixo!
int confereSimulaAcoes(Acao acoes[], int n, int melhor_dia, float maior_valor);




//escreva as suas funcoes abaixo:
//(voce pode criar funcoes extras, se preferir)

//funcoes extras

float randf() { 
    return (float)rand()/RAND_MAX; 
} 

int positivoOuNegativo() {

    int i = rand()%2;
    int j = -1;

    if (i == 1) {
        return i;
    } else {
        return j;
    }
}

void zeraMatriz(float p[][NUM_DIAS], int q, int r) {

    for(int i = 0; i < q; i++) {
        for (int j = 0; j < r; j++) {
            p[i][j] = 0;
        }
    } 
}

//1)

/*
 Implemente uma função de nome randVar que recebe um número ponto flutuante x como parâmetro e 
 retorna um número ponto flutuante aleatório que tenha no máximo 1/10 (um décimo) 
 do valor de x de diferença para x. Exemplo: se x é 100, 
 então você deve gerar um número aleatório entre 90 e 110 (1/10 de 100 = 10). 
 A sua função deve dar igual probabilidade para todos os números que podem ser gerados 
 com probabilidade positiva dentro desse intervalo 
 (como o espaço é contínuo e há limites de precisão de ponto flutuante, 
 então alguns números no intervalo não poderão ser gerados). 
 Exemplo: se sua função simplesmente retornar x, o número gerado estará no intervalo, 
 mas outros números (ex: 90) podem ser gerados e nunca serão gerados. 
 
 Importante: essa regra é válida para todas as funções de geração de valores aleatórios nesta prova! 
 Use o #define FRAC_VAR neste exercício.
*/

float randVar(float x) {

    float aleatorio = 0;
    float x_maior = 0, x_menor = 0;
    x_maior = x + x * FRAC_VAR;
    x_menor = x - x * FRAC_VAR;

    do {
        aleatorio = rand() % (int)(x_maior - x_menor);
        aleatorio += x_menor + randf()*(x_maior - x_menor);

    } while (aleatorio < x_menor || aleatorio > x_maior);
    

    return aleatorio;
}

//2)

/*
Implemente uma função de nome criaNome que recebe um vetor de caracteres e o seu tamanho n como parâmetros 
e o preenche com uma string aleatória. Essa string representa o nome de uma ação e 
é preenchida da seguinte maneira: 
os primeiros n-1 caracteres devem ser preenchidos com letras maiúsculas aleatórias e 
o n-ésimo caractere com um número aleatório (entre 0 e 9, inclusive para ambos). 
Assuma que n é sempre maior ou igual a 2 e 
que não haverá problemas de invasão de memória para preencher a string.
*/

void criaNome(char s[], int n) {

	int i = 0;

	for ( i = 0; i < n-1; i++ ) {
		s[i] = 'A' + (char)(rand()%26);
	}
    s[n-1] = '0' + (char)(rand()%10);
    s[n] = '\0';
    //printf ( " Nome: %s\n", s );

}

//3)

/*
Implemente uma função de nome criaAcao que recebe uma Acao como parâmetro por referência e preenche seus campos. 
Para o vetor precos, considere que precos[0] é o dia em que a ação foi comprada 
e precos[1] a precos[n-1] correspondem aos dias seguintes à compra. 
Preencha esse vetor da seguinte maneira: no dia da compra, 
a ação tem um valor aleatório entre 5 (usar o #define ACAO_INI_MIN) e 100 (usar o #define ACAO_INI_MAX). 
Os dias seguintes devem ser preenchidos com valores aleatórios de forma que 
o preço no dia k sofra uma variação de no máximo FRAC_VAR em relação ao dia k-1. 
Os campos max e min devem ser preenchidos com o valor máximo e mínimo gerado no vetor precos, respectivamente. 
O nome deve ser preenchido a partir da função criaNome. 
*/

void criaAcao(Acao *a) {

    int i = 0;
    float p_maior = 0, p_menor = 0;
    a->max = ACAO_INI_MIN;
    a->min = ACAO_INI_MAX;

    criaNome(a->nome,TAM_NOME);

    do {
        a->precos[0] = rand() % (int)(ACAO_INI_MAX - ACAO_INI_MIN);
        a->precos[0] += ACAO_INI_MIN + randf();

    } while (a->precos[0] < ACAO_INI_MIN || a->precos[0] > ACAO_INI_MAX);
    printf(" Preco[0]: %f\n", a->precos[0]);

    if (a->precos[0] > a->max) {
        a->max = a->precos[0];
    }
    if(a->precos[0] < a->min) {
        a->min = a->precos[0];
    }

    for ( i = 1; i < NUM_DIAS; i++) {
        do {
            a->precos[i] = positivoOuNegativo() * (rand() % (int)((a->precos[i-1] + a->precos[i-1] * FRAC_VAR) - (a->precos[i-1] - a->precos[i-1] * FRAC_VAR)));
            a->precos[i] += a->precos[i-1] + randf();
        } while (a->precos[i] < a->precos[i-1] - a->precos[i-1] * FRAC_VAR || a->precos[i] > a->precos[i-1] + a->precos[i-1] * FRAC_VAR);
        if (a->precos[i] > a->max) {
            a->max = a->precos[i];
        }
        if(a->precos[i] < a->min) {
            a->min = a->precos[i];
        }
        printf(" Preco[%d]: %f\n", i, a->precos[i]); 
    }

    //printf(" Max: %f\n", a->max);
    //printf(" Min: %f\n", a->min);
    //printf("\n");
}

//4)
/*
Implemente uma função de nome melhorDia que recebe um portfólio de ações (um vetor de Acao todo preenchido) 
e seu tamanho n como parâmetros e retorna o índice do dia em que o portfólio tem o seu maior valor. 
Considere que todas as ações do portfólio foram compradas no mesmo dia, ou seja, 
no dia de índice 0 para todas as ações contidas nele. 
Em outras palavras, a sua função deve retornar um número entre 0 (dia da compra) e NUM_DIAS-1 (último dia).
*/

int melhorDia(Acao portfolio[], int n) {

    int indice = 0;
    float p[n][NUM_DIAS];
    float soma = 0, aux = 0;

    zeraMatriz(p, n, NUM_DIAS);

    for (int j = 0; j < n; j++) {
        for (int i = 0; i < NUM_DIAS; i++) {
            p[j][i] += portfolio[j].precos[i];
            
            printf("precos port[%d][%d]: %f\n",j,i, portfolio[j].precos[i]);
            printf("precos p[%d][%d]: %f\n",j,i, p[j][i]);
        } 
        
    }

    for (int i = 0; i < NUM_DIAS; i++) {
        
        soma = 0;
    
        for (int j = 0; j < n; j++) {
            soma += p[j][i];
        }
        if (soma > aux) {
            indice = i;
            aux = soma;
        }
        
        
        printf("soma[%d]: %f\n",i, soma);
        printf(" indice: %d\n\n", indice);
    }
    
    //imprimeMatriz(p, n, NUM_DIAS);

    printf("\n");
    
    printf(" indice final: %d\n\n", indice);
    return indice;
}

//5)
/*
Implemente uma função de nome  maiorLucroPossivel que recebe um portfólio de ações 
(um vetor de Acao todo preenchido) e seu tamanho n como parâmetros e 
retorna o maior lucro possível para a sua venda. 
Considere que todas as ações do portfólio foram compradas no mesmo dia (dia 0) e 
também serão todas vendidas quando o portfólio for vendido. 
O lucro é dado pela diferença entre o valor total da venda e o valor total da compra.  
*/

float maiorLucroPossivel(Acao portfolio[], int n) {
    float maior_lucro = 0;
    float valor_venda = 0;
    float valor_compra = 0;

    float p[n][NUM_DIAS];
    float soma = 0, aux = 0;

    zeraMatriz(p, n, NUM_DIAS);

    for (int j = 0; j < n; j++) {
        for (int i = 0; i < NUM_DIAS; i++) {
            p[j][i] += portfolio[j].precos[i];
        } 
    }

    for (int i = 0; i < NUM_DIAS; i++) {
        soma = 0;
        for (int j = 0; j < n; j++) {
            soma += p[j][i];
        }
        if (soma > aux) {
            valor_venda = soma;
            aux = soma;
        }
    }

    for (int j = 0; j < n; j++) {
        valor_compra += p[j][0];
    }

    maior_lucro = valor_venda - valor_compra;

    printf("venda: %f\n", valor_venda);
    printf("compra: %f\n", valor_compra);
    printf("lucro: %f\n", maior_lucro);

    return maior_lucro;
}

//6)
/* 
Implemente uma função de nome  simulaAcoes que recebe um inteiro n como parâmetro e 
simula a criação de um portfólio de n ações. As n ações devem ser criadas usando a função criaAcao. 
Depois, você deve calcular o dia em que esse portfólio obteve o seu maior valor e 
também o lucro possível com a venda desse portfólio nesse dia. 
Essa função deve retornar o resultado dado pela função confereSimulaAcoes, que já está implementada. 
Em outras palavras, você deve chamar a função confereSimulaAcoes passando como parâmetros 
o portfólio que você criou (o vetor de ações), o seu tamanho n, 
o melhor dia e o maior lucro possível que vocês calculou. 
Essa função retorna um número inteiro e a sua função simulaAcoes deve retornar esse número.  
O protótipo da função confereSimulaAcoes  está especificado no início do arquivo da sua prova. 

Para essa função, use o #define MAX_ACOES como tamanho máximo e inicial do vetor de ações, 
já que ainda não aprendemos a fazer alocação dinâmica de memória, ou seja, 
não aprendemos a criar um vetor com tamanho definido durante a execução. 

E considere que n será sempre menor ou igual a MAX_ACOES.
*/

int simulaAcoes(int n) {

    if (n > MAX_ACOES) {
        n = MAX_ACOES;
    }
    
    
    Acao portfolio[n];

    for (int i = 0; i < n; i++) {
        criaAcao(&portfolio[i]); 
    }

    int melhor_dia = melhorDia(portfolio, n);
    float maior_valor = maiorLucroPossivel(portfolio, n);

    //return confereSimulaAcoes(portfolio, n, melhor_dia, maior_valor);
    return 0;
}

//---------------------------------------------------------------------------------------------------

int main() {
    
    srand(time(NULL));
    //int n = 1 + rand()%5;
    simulaAcoes(2);
    
    /*
   // teste 1
   srand(time(NULL));
   float a = 100;
   printf("\n Teste 1\n");
   printf("\n numero original: %.2f\n numero aleatorio: %.2f\n", a, randVar(a));
   

    // Teste 2
    int n = 6;
    char b[n];

    printf("\n Teste 2\n");
    printf("\n Tamanho do nome: %d\n", n);
    criaNome(b, n);
    
    // Teste 3
    Acao c;
    printf("\n Teste 3\n\n");
    criaAcao(&c);
    
    // Teste 4
    printf("\n Teste 4\n\n");

    // Teste 5
    printf("\n Teste 5\n\n");

    // Teste 6
    printf("\n Teste 6\n\n");

    simulaAcoes(2);
    */
    return 0;
}