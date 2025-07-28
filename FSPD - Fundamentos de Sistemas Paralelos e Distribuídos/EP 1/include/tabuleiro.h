#ifndef TABULEIRO_H
#define TABULEIRO_H

#include <pthread.h>

#define MAX_N 20

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int ocupantes;   // quantas threads estão na celula
    int grupo;       // grupo atual (se > 0 threads)
} Celula;

extern Celula tabuleiro[MAX_N][MAX_N];

// Inicializa o tabuleiro
void inicializaTabuleiro(int dimensao_N);

// Destroi o tabuleiro
void destroiTabuleiro(int dimensao_N);

// Thread tenta entrar numa posição do tabuleiro
void entraCelula(int x, int y, int grupo);

// Thread libera a posição após seu tempo de permanência
void saiCelula(int x, int y);


#endif