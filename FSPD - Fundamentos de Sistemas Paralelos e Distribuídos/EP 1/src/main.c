#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "tabuleiro.h"
#include "passa_tempo.c"

#define MAX_N 20         // Tamanho máximo do tabuleiro
#define MAX_TRAJETO 40   // Número máximo de posições que uma thread pode percorrer

// Representa uma posição no trajeto da thread
typedef struct {
    int x, y;       // Coordenadas da posição no tabuleiro
    int tempo;      // Tempo que a thread permanece nesta posição
} Posicao;

// Informações associadas a cada thread
typedef struct {
    int indice;                     // Índice da thread (usado internamente)
    int identificador;              // Identificador da thread
    int grupo;                      // Grupo da thread 
    int num_posicoes;               // Quantas posições o trajeto contém
    Posicao trajeto[MAX_TRAJETO];   // Trajeto completo da thread
} InfoThread;

// Função executada por cada thread
void* threadFunc(void* arg) {

    InfoThread* info = (InfoThread*) arg;
    int identificador = info->identificador;
    int grupo = info->grupo;

    // Primeira posição: entra na célula e executa a ação
    int x = info->trajeto[0].x;
    int y = info->trajeto[0].y;
    int tempo = info->trajeto[0].tempo;

    // Tenta entrar na célula (com sincronização)
    entraCelula(x, y, grupo);
    passa_tempo(identificador, x, y, tempo);

    // Percorre as demais posições do trajeto
    for (int i = 1; i < info->num_posicoes; i++) {
        int x_novo = info->trajeto[i].x;
        int y_novo = info->trajeto[i].y;
        int tempo = info->trajeto[i].tempo;

        // Tenta entrar na nova célula (sincronizado)
        entraCelula(x_novo, y_novo, grupo);

        // Libera a célula anterior
        saiCelula(x, y);

        // Executa ação na nova posição
        passa_tempo(identificador, x_novo, y_novo, tempo);

        // Atualiza coordenadas para a próxima iteração
        x = x_novo;
        y = y_novo;
    }

    // Libera a última célula utilizada pela thread
    saiCelula(x, y);

    return NULL;
}

int main() {

    int dimensao_N, num_threads;
    InfoThread* info_threads;

    // Lê a dimensão do tabuleiro e a quantidade de threads da entrada padrão
    if (scanf("%d %d", &dimensao_N, &num_threads) != 2) {
        fprintf(stderr, "Erro ao ler dimensao N e numero de threads\n");
        return 1;
    }

    // Inicializa o tabuleiro com mutexes e variáveis de condição por célula
    inicializaTabuleiro(dimensao_N);

    // Aloca memória para armazenar informações de cada thread
    info_threads = (InfoThread*) malloc(num_threads * sizeof(InfoThread));
    if (info_threads == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para as threads\n");
        return 1;
    }

    // Lê os dados do trajeto de cada thread a partir do arquivo
    for (int i = 0; i < num_threads; i++) {
        info_threads[i].indice = i;

        if (scanf("%d %d %d",
                    &info_threads[i].identificador,
                    &info_threads[i].grupo,
                    &info_threads[i].num_posicoes) != 3) {
            fprintf(stderr, "Erro ao ler dados da thread %d\n", i);
            free(info_threads);
            return 1;
        }


        // Lê as posições do trajeto dessa thread
        for (int j = 0; j < info_threads[i].num_posicoes; j++) {
            if (scanf("%d %d %d",
                      &info_threads[i].trajeto[j].x,
                      &info_threads[i].trajeto[j].y,
                      &info_threads[i].trajeto[j].tempo) != 3) {
                fprintf(stderr, "Erro ao ler posicao %d da thread %d\n", j, info_threads[i].identificador);
                free(info_threads);
                return 1;
            }
        }
    }

    // Aloca o vetor de identificadores das threads POSIX
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para threads\n");
        free(info_threads);
        return 1;
    }

    // Cria as threads com os dados lidos
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, threadFunc, &info_threads[i]) != 0) {
            perror("Erro ao criar thread");
            free(info_threads);
            free(threads);
            return 1;
        }
    }

    // Espera todas as threads terminarem sua execução
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Libera recursos do tabuleiro (mutexes e cond vars)
    destroiTabuleiro(dimensao_N);

    // Libera memória alocada para os dados das threads e os identificadores
    free(info_threads);
    free(threads);

    return 0;
}