/*
* Estratégia de Sincronização:
* Cada célula do tabuleiro possui seu próprio mutex e variável de condição.
* A entrada em uma célula é permitida apenas para threads do mesmo grupo.
* Se uma célula estiver ocupada por outro grupo, a thread é bloqueada em
* uma variável de condição até que a célula seja liberada (sem ocupantes).
* 
* Isso garante:
* - Exclusão mútua entre grupos diferentes por célula
* - Acesso concorrente de threads do mesmo grupo
* - Ausência de espera ativa (busy-waiting)
*/

#include "tabuleiro.h"

Celula tabuleiro[MAX_N][MAX_N];

// Inicializa todas as células do tabuleiro com mutexes e variáveis de condição
void inicializaTabuleiro(int dimensao_N) {
    for (int i = 0; i < dimensao_N; i++) {
        for (int j = 0; j < dimensao_N; j++) {

            // Cada célula possui seu próprio mutex para controle de acesso individual
            pthread_mutex_init(&tabuleiro[i][j].mutex, NULL);

            // Cada célula possui também sua própria variável de condição,
            // que permite que threads aguardem por disponibilidade
            pthread_cond_init(&tabuleiro[i][j].cond, NULL);

            // Nenhum thread está na célula no início
            tabuleiro[i][j].ocupantes = 0;

            // Grupo -1 indica que a célula está livre (sem grupo dominante)
            tabuleiro[i][j].grupo = -1;
        }
    }
}

// Libera recursos de sincronização (mutex e cond vars) usados por cada célula
void destroiTabuleiro(int dimensao_N) {
    for (int i = 0; i < dimensao_N; i++) {
        for (int j = 0; j < dimensao_N; j++) {

            // Destrói o mutex associado à célula
            pthread_mutex_destroy(&tabuleiro[i][j].mutex);

            // Destrói a variável de condição associada à célula
            pthread_cond_destroy(&tabuleiro[i][j].cond);
        }
    }
}

// Thread tenta entrar numa posição do tabuleiro
void entraCelula(int x, int y, int grupo) {
    Celula* cel = &tabuleiro[x][y];

    // Protege a célula com mutex para garantir exclusão mútua ao acessar/modificar seus dados
    pthread_mutex_lock(&cel->mutex);
    
    // Aguarda se a célula já está ocupada por outro grupo.
    // Threads do mesmo grupo podem compartilhar a célula simultaneamente.
    while (cel->ocupantes > 0 && cel->grupo != grupo) {
        // Espera ser sinalizada quando a célula for liberada
        pthread_cond_wait(&cel->cond, &cel->mutex);
    }
    
    // A thread entra na célula: atualiza o grupo e incrementa ocupantes
    cel->ocupantes++;
    cel->grupo = grupo;

    pthread_mutex_unlock(&cel->mutex);
}

// Thread libera a posição após seu tempo de permanência
void saiCelula(int x, int y) {
    Celula* cel = &tabuleiro[x][y];

    // Protege a célula com mutex para garantir exclusão mútua ao acessar/modificar seus dados
    pthread_mutex_lock(&cel->mutex);
    
    // Decrementa o número de ocupantes
    cel->ocupantes--;
    
    // Se a última thread saiu, o grupo da célula é resetado
    // e todas as threads bloqueadas na cond var são notificadas
    if (cel->ocupantes == 0) {
        cel->grupo = -1;
        pthread_cond_broadcast(&cel->cond); // Acorda todas as threads esperando
    }

    pthread_mutex_unlock(&cel->mutex);
}