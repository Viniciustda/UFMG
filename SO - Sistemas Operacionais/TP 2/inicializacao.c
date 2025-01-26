#include "inicializacao.h"
#include "structs.h"

// -------------------------------------------------- Funções de Inicialização --------------------------------------------------

void iniciaEstadoSimulacao(estadoSimulacao *estado, int tam_total_mem, int tam_pagina, int num_bits_endereco) {
    estado->tam_memoria = tam_total_mem / tam_pagina;
    estado->atual_tam_memoria = 0;
    estado->tam_tabela_paginas = (1 << num_bits_endereco);
    estado->tam_total_mem = tam_total_mem;

    estado->pageFaults = 0;
    estado->acessos_disco = 0;
    estado->acessos_memoria = 0;
    estado->acessos_mem_R = 0;
    estado->acessos_mem_W = 0;
    estado->paginas_sujas = 0;

    estado->memoria = (quadro *)malloc(estado->tam_memoria * sizeof(quadro));
    for (int i = 0; i < estado->tam_memoria; i++) {
        estado->memoria[i].endereco_tabela = -1;
        estado->memoria[i].sujo = 0;
        estado->memoria[i].ultimo_acesso = 0;
        estado->memoria[i].referencia = 0;
    }

    estado->tabela_paginas = (pagina *)malloc(estado->tam_tabela_paginas * sizeof(pagina));
    for (int i = 0; i < estado->tam_tabela_paginas; i++) {
        estado->tabela_paginas[i].endereco_memoria = -1;
        estado->tabela_paginas[i].valido = 0;
        estado->tabela_paginas[i].sujo = 0;
    }

    estado->tabela_paginas_invertida = (invertida *)malloc(estado->tam_memoria * sizeof(invertida));
    for (int i = 0; i < estado->tam_memoria; i++) {
        estado->tabela_paginas_invertida[i].endereco_memoria = -1;
        estado->tabela_paginas_invertida[i].endereco_virtual = -1;
        estado->tabela_paginas_invertida[i].valido = 0;
        estado->tabela_paginas_invertida[i].sujo = 0;
    }

    estado->bits_n1 = num_bits_endereco / 2;
    estado->bits_n2 = num_bits_endereco - estado->bits_n1;
    int tam_n1 = 1 << estado->bits_n1;

    estado->tabela_pagina_n2 = (pagina **)malloc(tam_n1 * sizeof(pagina *));
    for (int i = 0; i < tam_n1; i++) {
        estado->tabela_pagina_n2[i] = NULL;
    }

    estado->bits_n1 = num_bits_endereco / 3;
    estado->bits_n2 = num_bits_endereco / 3;
    estado->bits_n3 = num_bits_endereco - estado->bits_n1 - estado->bits_n2;
    tam_n1 = 1 << estado->bits_n1;

    estado->tabela_pagina_n3 = (pagina ***)malloc(tam_n1 * sizeof(pagina **));
    for (int i = 0; i < tam_n1; i++) {
        estado->tabela_pagina_n3[i] = NULL;
    }

    estado->lista_id = malloc(sizeof(lista));
    estado->lista_id->primeiro = NULL;
    estado->lista_id->ultimo = NULL;
    estado->lista_id->tamanho = 0;

    estado->lista_circular = malloc(sizeof(lista));
    estado->lista_circular->primeiro = NULL;
    estado->lista_circular->ultimo = NULL;
    estado->lista_circular->tamanho = 0;
}

void inicializaTabelaPaginas(estadoSimulacao *estado){
    estado->tabela_paginas = (pagina *)malloc(estado->tam_tabela_paginas * sizeof(pagina));
    for(int i = 0; i < estado->tam_tabela_paginas; i++){
        estado->tabela_paginas[i].endereco_memoria = -1;
        estado->tabela_paginas[i].valido = 0;
        estado->tabela_paginas[i].sujo = 0;
    }
}

void inicializaTabelaPaginasInvertida(estadoSimulacao *estado){
    estado->tabela_paginas_invertida = (invertida *)malloc(estado->tam_memoria * sizeof(invertida));
    for(int i = 0; i < estado->tam_memoria; i++){
        estado->tabela_paginas_invertida[i].endereco_memoria = -1;
        estado->tabela_paginas_invertida[i].endereco_virtual = -1;
        estado->tabela_paginas_invertida[i].valido = 0;
        estado->tabela_paginas_invertida[i].sujo = 0;
    }
}

void inicializaN1TabelaN2(estadoSimulacao *estado) {
    estado->bits_n1 = estado->num_bits_endereco / 2;
    estado->bits_n2 = estado->num_bits_endereco - estado->bits_n1;
    int tam_n1 = 1 << estado->bits_n1;

    estado->tabela_pagina_n2 = (pagina **)malloc(tam_n1 * sizeof(pagina *));
    for (int i = 0; i < tam_n1; i++) {
        estado->tabela_pagina_n2[i] = NULL;
    }
}

void inicializaN2TabelaN2(estadoSimulacao *estado, int id) {
    int tam_n2 = 1 << estado->bits_n2;
    estado->tabela_pagina_n2[id] = (pagina *)malloc(tam_n2 * sizeof(pagina));
    for (int i = 0; i < tam_n2; i++) {
        estado->tabela_pagina_n2[id][i].endereco_memoria = -1;
        estado->tabela_pagina_n2[id][i].valido = 0;
        estado->tabela_pagina_n2[id][i].sujo = 0;
    }
}

void inicializaN1TabelaN3(estadoSimulacao *estado) {
    estado->bits_n1 = estado->num_bits_endereco / 3;
    estado->bits_n2 = estado->num_bits_endereco / 3;
    estado->bits_n3 = estado->num_bits_endereco - estado->bits_n1 - estado->bits_n2;
    int tam_n1 = 1 << estado->bits_n1;

    estado->tabela_pagina_n3 = (pagina ***)malloc(tam_n1 * sizeof(pagina **));
    for (int i = 0; i < tam_n1; i++) {
        estado->tabela_pagina_n3[i] = NULL;
    }
}

void inicializaN2TabelaN3(estadoSimulacao *estado, int n1_id) {
    int tam_n2 = 1 << estado->bits_n2;
    estado->tabela_pagina_n3[n1_id] = (pagina **)malloc(tam_n2 * sizeof(pagina *));
    for (int i = 0; i < tam_n2; i++) {
        estado->tabela_pagina_n3[n1_id][i] = NULL;
    }
}

void inicializaN3TabelaN3(estadoSimulacao *estado, int n1_id, int n2_id) {
    int tam_n3 = 1 << estado->bits_n3;
    estado->tabela_pagina_n3[n1_id][n2_id] = (pagina *)malloc(tam_n3 * sizeof(pagina));
    for (int i = 0; i < tam_n3; i++) {
        estado->tabela_pagina_n3[n1_id][n2_id][i].endereco_memoria = -1;
        estado->tabela_pagina_n3[n1_id][n2_id][i].valido = 0;
        estado->tabela_pagina_n3[n1_id][n2_id][i].sujo = 0;
    }
}

void inicializaMemoria(estadoSimulacao *estado){
    estado->memoria = (quadro *)malloc(estado->tam_memoria * sizeof(quadro));
    for(int i = 0; i < estado->tam_memoria; i++){
        estado->memoria[i].endereco_tabela = -1;
        estado->memoria[i].sujo = 0;
        estado->memoria[i].ultimo_acesso = estado->acessos_memoria;
        estado->memoria[i].referencia = 0;
    }
}

void inicializaListaAux(lista **lista){
    *lista = malloc(sizeof(lista));
    (*lista)->tamanho = 0;
    (*lista)->primeiro = NULL;
    (*lista)->ultimo = NULL;
}

void liberaEstadoSimulacao(estadoSimulacao *estado) {
    // Libera memória principal
    if (estado->memoria) {
        free(estado->memoria);
        estado->memoria = NULL;
    }

    // Libera tabela de páginas
    if (estado->tabela_paginas) {
        free(estado->tabela_paginas);
        estado->tabela_paginas = NULL;
    }

    // Libera tabela de páginas invertida
    if (estado->tabela_paginas_invertida) {
        free(estado->tabela_paginas_invertida);
        estado->tabela_paginas_invertida = NULL;
    }

    // Libera tabelas hierárquicas (2 níveis)
    if (estado->tabela_pagina_n2) {
        int tam_n1 = 1 << estado->bits_n1;
        for (int i = 0; i < tam_n1; i++) {
            if (estado->tabela_pagina_n2[i]) {
                free(estado->tabela_pagina_n2[i]);
            }
        }
        free(estado->tabela_pagina_n2);
        estado->tabela_pagina_n2 = NULL;
    }

    // Libera tabelas hierárquicas (3 níveis)
    if (estado->tabela_pagina_n3) {
        int tam_n1 = 1 << estado->bits_n1;
        for (int i = 0; i < tam_n1; i++) {
            if (estado->tabela_pagina_n3[i]) {
                int tam_n2 = 1 << estado->bits_n2;
                for (int j = 0; j < tam_n2; j++) {
                    if (estado->tabela_pagina_n3[i][j]) {
                        free(estado->tabela_pagina_n3[i][j]);
                    }
                }
                free(estado->tabela_pagina_n3[i]);
            }
        }
        free(estado->tabela_pagina_n3);
        estado->tabela_pagina_n3 = NULL;
    }

    // Libera lista ID
    if (estado->lista_id) {
        node *atual = estado->lista_id->primeiro;
        while (atual) {
            node *proximo = atual->proximo;
            free(atual);
            atual = proximo;
        }
        free(estado->lista_id);
        estado->lista_id = NULL;
    }

    // Libera lista circular
    if (estado->lista_circular) {
        node *atual = estado->lista_circular->primeiro;
        if (atual) {
            do {
                node *proximo = atual->proximo;
                free(atual);
                atual = proximo;
            } while (atual != estado->lista_circular->primeiro);
        }
        free(estado->lista_circular);
        estado->lista_circular = NULL;
    }
}
