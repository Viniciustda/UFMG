#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

//Nao modificar o struct abaixo! Voce vai precisar dele no Exercicio 6.
typedef struct Stats {
	float a_idade_media;
	float b_maior_IMC;
	char c_nome_MVP[1000];	
} Stats;

//funcao opcional: implemente ela para visualizar a sua matriz diagonal do Exercicio 4.
//n = numero de linhas da matriz.
void imprimeMatrizDiag(int **M_diag, int n) {
	printf("**imprimeMatrizDiag: implemente esta funcao para visualizar a sua matriz diagonal**");
	printf("\n(opcional, ou seja, nao vale ponto)\n");
	int aux = n;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < aux; j++)
            printf(" %d", (M_diag)[i][j]);
        printf("\n");
        aux--;
    }
}

//------------A1------------

int fatorial(int n) {
    if(n==0) {
        return 1;
    } else {
        return n * fatorial(n-1);
    }
}

float euler(int n) {
    if(n==1) {
        return 1.0;
    }else {
        return (1.0 / fatorial(n-1)) + euler(n-1);
    }
}

//------------A2------------

int potenciaDe2(int n) {
    if(n==0) {
        return 1;
    }else {
        return potenciaDe2(n-1)*2;
    }
}

float serieDoisTercos(int n) {
    if(n==1) {
        return 1.0;
    }else {
        if ((n-1)%2 == 0)
        {
            return  1.0/potenciaDe2(n-1)+serieDoisTercos(n-1);
        }
        else
        {
            return  (-1.0)/potenciaDe2(n-1)+serieDoisTercos(n-1);
        }
    }
}


//------------A3------------
//funcionou aqui:
int caixaAlta(char *str) {
   if(str[0]=='\0') {  
        return 0;                                     
    }
    else {
        if((str[0] >= 'A' && str[0] <= 'Z') || str[0] == ' '){
           return caixaAlta(str+1);
        }else {
            str[0] -= (char)32;
            return caixaAlta(str+1) + 1; 
        }   
    }
}
//funcionou na prova:
/*
int caixaAlta(char *str) {
   if(str[1]=='\0') {  
        return 0;                                     
    }
    else {
        if((str[0] >= 'A' && str[0] <= 'Z') || str[0] == ' '){
           return caixaAlta(str+1);
        }else {
            str[0] -= (char)32;
            return caixaAlta(str+1) + 1; 
        }   
    }
}
*/

//------------A4------------

void aloca_matriz (int ***MD, int n, int inicial) {
    int aux = n;
    *MD = (int **) malloc(n * sizeof(int *));

    for (int i = 0; i < n; i++) {
        (*MD)[i] = (int *) malloc(n * sizeof(int));
        for (int j = 0; j < aux; j++) {
            (*MD)[i][j] = inicial;
        }
        aux--;
    }
}

void preenche_matriz (int ***MD, int n, int ***M) {
    int aux = n;
    int aux2 = 0;
    int amgs = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if((*M)[i][j] == 1)
                amgs++;
        }
        (*MD)[i][0] = amgs;
        amgs = 0;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 1; j < aux; j++) {
            (*MD)[i][j] = (*M)[i][j+aux2];
        }
        aux--;
        aux2++;
    }
}

void libera_matriz (int ***mat, int n) {
    for (int i = 0; i < n; i++)
        free((*mat)[i]);
    free(*mat);
}

int** matrizDiag(int **M, int n) {
    int **MD;
    int inicial = 0;

    aloca_matriz(&MD, n, inicial);

    preenche_matriz(&MD, n, &M);

    imprimeMatrizDiag(MD, n);

    libera_matriz(&M, n);
    
    return MD;

}

//------------A5------------

int saoAmigos(int **M, int i, int j) {
    
    if(i < j){
        return M[i][j-i];

    }else if(i > j){
        return M[j][i-j];

    }else {
        return 0;
    }
}

//------------A6------------

Stats estatisticasNBA() {
    Stats resposta;

    int tamanho = 200000;

    char *nome = NULL;
    char *time_abreviado = NULL;
    int idade = 0;
    float altura = 0.0;
    float peso = 0.0;
    char *universidade = NULL;
    char *pais = NULL;
    int num_jogos = 0;
    float pontos = 0.0;
    float rebotes = 0.0;
    float assistencias = 0.0;
    char *temporada = NULL;

    float idade_media_chi = 0.0;
    int idade_total_chi = 0;
    int n_jogadores_chi = 0;

    float imc = 0.0;
    float maior_imc = 0.0;
    float altura_m = 0.0;

    float eficiencia = 0.0;
    float maior_eficiencia = 0.0;
    char MVP[1000];

    FILE *arq = fopen("nba.csv", "r"); 

    char linha[tamanho];
    fgets(linha, tamanho, arq);

    while(!feof(arq)) {
        fgets(linha, tamanho, arq);

        nome = strtok(linha, ",");
        time_abreviado = strtok(NULL, ",");
        idade = atoi(strtok(NULL, ","));
        altura = atof(strtok(NULL, ","));
        peso = atof(strtok(NULL, ","));
        universidade = strtok(NULL, ",");
        pais = strtok(NULL, ",");
        num_jogos = atoi(strtok(NULL, ","));
        pontos = atof(strtok(NULL, ","));                               
        rebotes = atof(strtok(NULL, ","));                             
        assistencias = atof(strtok(NULL, ","));                     
        temporada = strtok(NULL, ",");

        if(time_abreviado[0] == 'C' && time_abreviado[1] == 'H' && time_abreviado[2] == 'I') {
            idade_total_chi += idade;
            n_jogadores_chi++;
        }
        
        altura_m = altura/100;
        imc = peso/(altura_m*altura_m);
        if(imc > maior_imc) {
            maior_imc = imc;
        }

        eficiencia = (pontos+rebotes+assistencias)/(float)num_jogos;
        if(eficiencia > maior_eficiencia) {
            maior_eficiencia = eficiencia;
            strcpy(MVP, nome);
        }

    }
    
    idade_media_chi = (float)idade_total_chi/n_jogadores_chi;

    resposta.a_idade_media = idade_media_chi;
    resposta.b_maior_IMC = maior_imc;
    strcpy(resposta.c_nome_MVP , MVP);

    fclose(arq);

    return resposta;
}