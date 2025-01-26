#ifndef STRUCTS_H
#define STRUCTS_H

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -------------------------------------------------- Estruturas de Dados --------------------------------------------------

typedef struct pagina{
    int endereco_memoria;
    int valido; 
    int sujo;
}pagina;

typedef struct quadro{
    int endereco_tabela;
    int sujo;
    int ultimo_acesso;
    int referencia;
}quadro;

typedef struct invertida{
    int endereco_memoria;
    int endereco_virtual;
    int valido; 
    int sujo;
}invertida;

typedef struct node{
    struct node *proximo;
    struct node *anterior;
    int id;
} node;

typedef struct lista{
    int tamanho;
    struct node *primeiro;
    struct node *ultimo;
} lista;

typedef struct estadoSimulacao {
    quadro *memoria;
    int tam_memoria;
    int atual_tam_memoria;

    pagina *tabela_paginas;
    int tam_tabela_paginas;

    lista *lista_id;
    lista *lista_circular;

    invertida *tabela_paginas_invertida;

    pagina **tabela_pagina_n2;
    pagina ***tabela_pagina_n3;

    int bits_n1;
    int bits_n2;
    int bits_n3;
    int num_bits_endereco;

    int tam_total_mem;
    int pageFaults;
    int acessos_disco;
    int acessos_memoria;
    int acessos_mem_R;
    int acessos_mem_W;
    int paginas_sujas;
} estadoSimulacao;

#endif