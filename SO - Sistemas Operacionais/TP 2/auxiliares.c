#include "auxiliares.h"
#include <stdio.h>
#include <string.h>

// -------------------------------------------------- Funções Auxiliares --------------------------------------------------

void printParametros() {
    printf("Uso: tp2virtual <algoritmo> <arquivo_entrada> <tamanho_pagina> <tamanho_memoria> [tabela]\n");
    printf("  <algoritmo>: lru, fifo, random, 2a\n");
    printf("  <arquivo_entrada>: caminho para o arquivo de entrada\n");
    printf("  <tamanho_pagina>: tamanho da pagina em KB (potencia de 2 entre 2 e 64)\n");
    printf("  <tamanho_memoria>: tamanho da memoria em KB (potencia de 2 entre 128 e 16384)\n");
    printf("  [tabela]: opcional, densa, hierarquica2, hierarquica3, inversa\n");
}

int potenciaDeDois(int x) {
    return (x > 0) && ((x & (x - 1)) == 0);
}

int validaArgumentos(int argc, char *algoritmo, FILE *arquivo_entrada, int tam_pagina, int tam_total_mem, char *tp_tabela) {
    if (argc < 5 || argc > 6) {
        printParametros();
        return 1;
    }

    // Argumento 1: Algoritmo de substituição
    if (strcmp(algoritmo, "lru") != 0 && strcmp(algoritmo, "fifo") != 0 && strcmp(algoritmo, "random") != 0 && strcmp(algoritmo, "2a") != 0) {
        printf("Erro: Algoritmo invalido. Escolha entre: lru, fifo, random, 2a\n");
        return 1;
    }

    // Argumento 2: Arquivo de entrada
    if (!arquivo_entrada) {
        printf("Erro: Nao foi possivel abrir o arquivo de entrada.\n");
        return 1;
    }

    // Argumento 3: Tamanho da página
    if (!potenciaDeDois(tam_pagina) || tam_pagina < 2 || tam_pagina > 64) {
        printf("Erro: Tamanho da pagina deve ser uma potencia de 2 entre 2 e 64\n");
        return 1;
    }

    // Argumento 4: Tamanho da memória
    if (!potenciaDeDois(tam_total_mem) || tam_total_mem < 128 || tam_total_mem > 16384) {
        printf("Erro: Tamanho da memoria deve ser uma potencia de 2 entre 128 e 16384\n");
        return 1;
    }

    // Argumento 5 (opcional): Debug
    if (argc == 6) {
        if (strcmp(tp_tabela, "densa") != 0 && strcmp(tp_tabela, "hierarquica2") != 0 && strcmp(tp_tabela, "hierarquica3") != 0 && strcmp(tp_tabela, "inversa") != 0) {
            printf("Erro: Tabela invalida. Escolha entre: densa, hierarquica2, hierarquica3, inversa\n");
            return 1;
        }
    }

    return 0;
}

int determinaS(int tam_pagina){
    int s = 0;
    int tmp = tam_pagina * 1024;
    while(tmp > 1){
        tmp = tmp >> 1;
        s++;
    }
    return s;
}

void geraRelatorio(char *arquivo_entrada, int tam_total_mem, int tam_pagina, char *algoritmo, char *tp_tabela, int acessos_memoria, int page_faults, int acessos_mem_R, int acessos_mem_W, int paginas_sujas) {
    printf("\n=========== Relatorio de Execucao ===========\n\n");
    printf("Arquivo de entrada: %s\n", arquivo_entrada);
    printf("Tamanho da memoria: %d KB\n", tam_total_mem);
    printf("Tamanho das paginas: %d KB\n", tam_pagina);
    printf("Tecnica de reposicao: %s\n", algoritmo);
    printf("Tipo de tabela: %s\n", tp_tabela);
    printf("Numero total de acessos a memoria: %d\n", acessos_memoria);
    printf("Numero de page faults: %d\n", page_faults);
    printf("Numero total de acessos de leitura: %d\n", acessos_mem_R);
    printf("Numero total de acessos de escrita: %d\n", acessos_mem_W);
    printf("Numero total de paginas sujas: %d\n", paginas_sujas);
    printf("\n=============================================\n");
}