#include "algoritmos.h"
#include "structs.h"

// -------------------------------------------------- Funções de Algoritmos - Tabela Densa --------------------------------------------------

void densaFIFO(estadoSimulacao *estado, int endereco){
    pagina *tabela_paginas = estado->tabela_paginas;
    quadro *memoria = estado->memoria;

    tabela_paginas[endereco].valido = 1;
    tabela_paginas[endereco].endereco_memoria = estado->tam_memoria-1;
    
    if(tabela_paginas[memoria[0].endereco_tabela].sujo){
        estado->paginas_sujas++;
        tabela_paginas[memoria[0].endereco_tabela].sujo = 0;
    }
    tabela_paginas[memoria[0].endereco_tabela].valido = 0;
    
    for (int i = 0; i < estado->tam_memoria - 1; i++){
        memoria[i] = memoria[i+1];
    }
    memoria[estado->tam_memoria-1].endereco_tabela = endereco;
    memoria[estado->tam_memoria-1].ultimo_acesso = estado->acessos_memoria; 
}

void densaRandom(estadoSimulacao *estado, int endereco){
    pagina *tabela_paginas = estado->tabela_paginas;
    quadro *memoria = estado->memoria;

    int num_random = rand() % estado->tam_memoria;
    tabela_paginas[endereco].valido = 1;
    tabela_paginas[endereco].endereco_memoria = num_random;

    if(tabela_paginas[memoria[num_random].endereco_tabela].sujo){
        estado->paginas_sujas++;
        tabela_paginas[memoria[num_random].endereco_tabela].sujo = 0;
    }
    tabela_paginas[memoria[num_random].endereco_tabela].valido = 0;

    memoria[num_random].endereco_tabela = endereco;
    memoria[num_random].ultimo_acesso = estado->acessos_memoria; 
}  

void densaSegundaChance(estadoSimulacao *estado, int endereco){
    pagina *tabela_paginas = estado->tabela_paginas;
    quadro *memoria = estado->memoria;
    lista *lista_circular = estado->lista_circular;
    node *index = lista_circular->primeiro;

    while(memoria[index->id].referencia == 1){
        memoria[index->id].referencia = 0;
        index = index->proximo;
    }

    tabela_paginas[endereco].valido = 1;
    tabela_paginas[endereco].endereco_memoria = index->id;

    if(tabela_paginas[memoria[index->id].endereco_tabela].sujo){
        estado->paginas_sujas++;
        tabela_paginas[memoria[index->id].endereco_tabela].sujo = 0;
    }
    tabela_paginas[memoria[index->id].endereco_tabela].valido = 0;
    
    memoria[index->id].endereco_tabela = endereco;
    memoria[index->id].ultimo_acesso = estado->acessos_memoria;

    lista_circular->primeiro = index->proximo;
    lista_circular->primeiro = index;
}  

void densaLRU(estadoSimulacao *estado, int endereco){
    pagina *tabela_paginas = estado->tabela_paginas;
    quadro *memoria = estado->memoria;
    lista *lista_id = estado->lista_id;
    int id = lista_id->primeiro->id;
    
    tabela_paginas[endereco].valido = 1;
    tabela_paginas[endereco].endereco_memoria = id;

    if(tabela_paginas[memoria[id].endereco_tabela].sujo){
        estado->paginas_sujas++;
        tabela_paginas[memoria[id].endereco_tabela].sujo = 0;
    }
    tabela_paginas[memoria[id].endereco_tabela].valido = 0;

    memoria[id].endereco_tabela = endereco;
    memoria[id].ultimo_acesso = estado->acessos_memoria; 

    node *no = lista_id->primeiro;
    lista_id->primeiro = lista_id->primeiro->proximo;
    lista_id->primeiro->anterior = NULL;
    no->proximo = NULL;
    no->anterior = lista_id->ultimo;
    lista_id->ultimo->proximo = no;
    lista_id->ultimo = no;
}  

// -------------------------------------------------- Funções de Algoritmos - Tabela Invertida --------------------------------------------------

int invertidaFIFO(estadoSimulacao *estado, int endereco){
    quadro *memoria = estado->memoria;
    invertida *tabela_paginas_invertida = estado->tabela_paginas_invertida;
    quadro aux = memoria[0];

    for (int i = 0; i < estado->tam_memoria - 1; i++){
        memoria[i] = memoria[i+1];
        tabela_paginas_invertida[memoria[i+1].endereco_tabela].endereco_memoria --;
    }

    tabela_paginas_invertida[aux.endereco_tabela].endereco_memoria = estado->tam_memoria-1;
    tabela_paginas_invertida[aux.endereco_tabela].endereco_virtual = endereco;
    if(tabela_paginas_invertida[aux.endereco_tabela].sujo){
        estado->paginas_sujas++;
        tabela_paginas_invertida[aux.endereco_tabela].sujo = 0;
    }
    tabela_paginas_invertida[aux.endereco_tabela].valido = 1;
    memoria[estado->tam_memoria-1].endereco_tabela = aux.endereco_tabela;
    memoria[estado->tam_memoria-1].ultimo_acesso = estado->acessos_memoria; 

    return aux.endereco_tabela;
}

int invertidaRandom(estadoSimulacao *estado, int endereco){
    quadro *memoria = estado->memoria;
    invertida *tabela_paginas_invertida = estado->tabela_paginas_invertida;
    int num_random = rand() % estado->tam_memoria;

    tabela_paginas_invertida[memoria[num_random].endereco_tabela].endereco_virtual = endereco;
    if(tabela_paginas_invertida[memoria[num_random].endereco_tabela].sujo){
        estado->paginas_sujas++;
        tabela_paginas_invertida[memoria[num_random].endereco_tabela].sujo = 0;
    }
    memoria[num_random].ultimo_acesso = estado->acessos_memoria;

    return memoria[num_random].endereco_tabela;
}  

int invertidaSegundaChance(estadoSimulacao *estado, int endereco){
    quadro *memoria = estado->memoria;
    invertida *tabela_paginas_invertida = estado->tabela_paginas_invertida;
    lista *lista_circular = estado->lista_circular;
    node *indice = lista_circular->primeiro;

    while(memoria[indice->id].referencia == 1){
        memoria[indice->id].referencia = 0;
        indice = indice->proximo;
    }
    
    tabela_paginas_invertida[memoria[indice->id].endereco_tabela].endereco_memoria = indice->id;
    tabela_paginas_invertida[memoria[indice->id].endereco_tabela].endereco_virtual = endereco;

    if(tabela_paginas_invertida[memoria[indice->id].endereco_tabela].sujo){
        estado->paginas_sujas++;
        tabela_paginas_invertida[memoria[indice->id].endereco_tabela].sujo = 0;
    }
    tabela_paginas_invertida[memoria[indice->id].endereco_tabela].valido = 1;
    
    memoria[indice->id].endereco_tabela = memoria[indice->id].endereco_tabela;
    memoria[indice->id].ultimo_acesso = estado->acessos_memoria;

    lista_circular->primeiro = indice->proximo;
    lista_circular->primeiro = indice;

    return memoria[indice->id].endereco_tabela;
}  

int invertidaLRU(estadoSimulacao *estado, int endereco){
    quadro *memoria = estado->memoria;
    invertida *tabela_paginas_invertida = estado->tabela_paginas_invertida;
    lista *lista_id = estado->lista_id;
    int id = lista_id->primeiro->id;

    tabela_paginas_invertida[memoria[id].endereco_tabela].endereco_virtual = endereco;
    if(tabela_paginas_invertida[memoria[id].endereco_tabela].sujo){
        estado->paginas_sujas++;
        tabela_paginas_invertida[memoria[id].endereco_tabela].sujo = 0;
    }
    memoria[id].ultimo_acesso = estado->acessos_memoria;

    node *no = lista_id->primeiro;
    lista_id->primeiro = lista_id->primeiro->proximo;
    lista_id->primeiro->anterior = NULL;
    no->proximo = NULL;
    no->anterior = lista_id->ultimo;
    lista_id->ultimo->proximo = no;
    lista_id->ultimo = no;

    return memoria[id].endereco_tabela;
}  

int buscaTabelaInvertida(estadoSimulacao *estado, int endereco_tabela){
    for(int i=0; i<estado->tam_memoria; i++){
        if(estado->tabela_paginas_invertida[i].endereco_virtual == endereco_tabela)
            return i;
    }

    return -1;
}

// -------------------------------------------------- Funções para Obter IDs --------------------------------------------------

int getIdNivel2Tabela1(unsigned endereco, int offset, int bits_n1, int bits_n2) {
    int deslocamento_total = offset + bits_n2;
    unsigned endereco_deslocado = endereco >> deslocamento_total;
    unsigned mascara = (1 << bits_n1) - 1;
    return endereco_deslocado & mascara;
}

int getIdNivel2Tabela2(unsigned endereco, int offset, int bits_n2) {
    unsigned endereco_deslocado = endereco >> offset;
    unsigned mascara = (1 << bits_n2) - 1;
    return endereco_deslocado & mascara;
}

int getIdNivel3Tabela1(unsigned endereco, int offset, int bits_n1, int bits_n2, int bits_n3) {
    int deslocamento_total = offset + bits_n3 + bits_n2;
    unsigned endereco_deslocado = endereco >> deslocamento_total;
    unsigned mascara = (1 << bits_n1) - 1;
    return endereco_deslocado & mascara;
}

int getIdNivel3Tabela2(unsigned endereco, int offset, int bits_n2, int bits_n3) {
    int deslocamento_total = offset + bits_n3;
    unsigned endereco_deslocado = endereco >> deslocamento_total;
    unsigned mascara = (1 << bits_n2) - 1;
    return endereco_deslocado & mascara;
}

int getIdNivel3Tabela3(unsigned endereco, int offset, int bits_n3) {
    unsigned endereco_deslocado = endereco >> offset;
    unsigned mascara = (1 << bits_n3) - 1;
    return endereco_deslocado & mascara;
}

// -------------------------------------------------- Funções de Algoritmos - Tabela Hierárquica - 2 níveis --------------------------------------------------

void hierarquica2FIFO(estadoSimulacao *estado, int endereco, int offset){
    int n1_id = getIdNivel2Tabela1(endereco, offset, estado->bits_n1, estado->bits_n2);
    int n2_id = getIdNivel2Tabela2(endereco, offset, estado->bits_n2);

    estado->tabela_pagina_n2[n1_id][n2_id].valido = 1;
    estado->tabela_pagina_n2[n1_id][n2_id].endereco_memoria = estado->tam_memoria-1;

    int endereco_antigo = estado->memoria[0].endereco_tabela;
    int n1_id_antigo = getIdNivel2Tabela1(endereco_antigo, offset, estado->bits_n1, estado->bits_n2);
    int n2_id_antigo = getIdNivel2Tabela2(endereco_antigo, offset, estado->bits_n2);

    if(estado->tabela_pagina_n2[n1_id_antigo][n2_id_antigo].sujo){
        estado->paginas_sujas++;
        estado->tabela_pagina_n2[n1_id_antigo][n2_id_antigo].sujo = 0;
    }
    estado->tabela_pagina_n2[n1_id_antigo][n2_id_antigo].valido = 0;
    
    for (int i = 0; i < estado->tam_memoria - 1; i++){
        estado->memoria[i] = estado->memoria[i+1];
    }
    estado->memoria[estado->tam_memoria-1].endereco_tabela = endereco;
    estado->memoria[estado->tam_memoria-1].ultimo_acesso = estado->acessos_memoria; 
}

void hierarquica2Random(estadoSimulacao *estado, int endereco, int offset){
    int num_random = rand() % estado->tam_memoria;
    int n1_id = getIdNivel2Tabela1(endereco, offset, estado->bits_n1, estado->bits_n2);
    int n2_id = getIdNivel2Tabela2(endereco, offset, estado->bits_n2);

    estado->tabela_pagina_n2[n1_id][n2_id].valido = 1;
    estado->tabela_pagina_n2[n1_id][n2_id].endereco_memoria = num_random;

    int endereco_antigo = estado->memoria[num_random].endereco_tabela;
    int n1_id_antigo = getIdNivel2Tabela1(endereco_antigo, offset, estado->bits_n1, estado->bits_n2);
    int n2_id_antigo = getIdNivel2Tabela2(endereco_antigo, offset, estado->bits_n2);

    if(estado->tabela_pagina_n2[n1_id_antigo][n2_id_antigo].sujo){
        estado->paginas_sujas++;
        estado->tabela_pagina_n2[n1_id_antigo][n2_id_antigo].sujo = 0;
    }
    estado->tabela_pagina_n2[n1_id_antigo][n2_id_antigo].valido = 0;

    estado->memoria[num_random].endereco_tabela = endereco;
    estado->memoria[num_random].ultimo_acesso = estado->acessos_memoria; 
}  

void hierarquica2SegundaChance(estadoSimulacao *estado, int endereco, int offset){
    quadro *memoria = estado->memoria;
    lista *lista_circular = estado->lista_circular;
    node *indice = lista_circular->primeiro;

    while(memoria[indice->id].referencia == 1){
        memoria[indice->id].referencia = 0;
        indice = indice->proximo;
    }

    int n1_id = getIdNivel2Tabela1(endereco, offset, estado->bits_n1, estado->bits_n2);
    int n2_id = getIdNivel2Tabela2(endereco, offset, estado->bits_n2);

    estado->tabela_pagina_n2[n1_id][n2_id].valido = 1;
    estado->tabela_pagina_n2[n1_id][n2_id].endereco_memoria = indice->id;

    int endereco_antigo = memoria[indice->id].endereco_tabela;
    int n1_id_antigo = getIdNivel2Tabela1(endereco_antigo, offset, estado->bits_n1, estado->bits_n2);
    int n2_id_antigo = getIdNivel2Tabela2(endereco_antigo, offset, estado->bits_n2);

    if(estado->tabela_pagina_n2[n1_id_antigo][n2_id_antigo].sujo){
        estado->paginas_sujas++;
        estado->tabela_pagina_n2[n1_id_antigo][n2_id_antigo].sujo = 0;
    }
    estado->tabela_pagina_n2[n1_id_antigo][n2_id_antigo].valido = 0;
    
    memoria[indice->id].endereco_tabela = endereco;
    memoria[indice->id].ultimo_acesso = estado->acessos_memoria;

    lista_circular->primeiro = indice->proximo;
    lista_circular->primeiro = indice;
}  

void hierarquica2LRU(estadoSimulacao *estado, int endereco, int offset){
    quadro *memoria = estado->memoria;
    lista *lista_id = estado->lista_id;
    int id = lista_id->primeiro->id;
    int n1_id = getIdNivel2Tabela1(endereco, offset, estado->bits_n1, estado->bits_n2);
    int n2_id = getIdNivel2Tabela2(endereco, offset, estado->bits_n2);

    estado->tabela_pagina_n2[n1_id][n2_id].valido = 1;
    estado->tabela_pagina_n2[n1_id][n2_id].endereco_memoria = id;

    int endereco_antigo = memoria[id].endereco_tabela;
    int n1_id_antigo = getIdNivel2Tabela1(endereco_antigo, offset, estado->bits_n1, estado->bits_n2);
    int n2_id_antigo = getIdNivel2Tabela2(endereco_antigo, offset, estado->bits_n2);

    if(estado->tabela_pagina_n2[n1_id_antigo][n2_id_antigo].sujo){
        estado->paginas_sujas++;
        estado->tabela_pagina_n2[n1_id_antigo][n2_id_antigo].sujo = 0;
    }
    estado->tabela_pagina_n2[n1_id_antigo][n2_id_antigo].valido = 0;

    memoria[id].endereco_tabela = endereco;
    memoria[id].ultimo_acesso = estado->acessos_memoria; 

    node *no = lista_id->primeiro;
    lista_id->primeiro = lista_id->primeiro->proximo;
    lista_id->primeiro->anterior = NULL;
    no->proximo = NULL;
    no->anterior = lista_id->ultimo;
    lista_id->ultimo->proximo = no;
    lista_id->ultimo = no;
}

// -------------------------------------------------- Funções de Algoritmos - Tabela Hierárquica - 3 níveis --------------------------------------------------

void hierarquica3FIFO(estadoSimulacao *estado, int endereco, int offset){
    int n1_id = getIdNivel3Tabela1(endereco, offset, estado->bits_n1, estado->bits_n2, estado->bits_n3);
    int n2_id = getIdNivel3Tabela2(endereco, offset, estado->bits_n2, estado->bits_n3);
    int n3_id = getIdNivel3Tabela3(endereco, offset, estado->bits_n3);
    
    estado->tabela_pagina_n3[n1_id][n2_id][n3_id].valido = 1;
    estado->tabela_pagina_n3[n1_id][n2_id][n3_id].endereco_memoria = estado->tam_memoria - 1;

    int endereco_antigo = estado->memoria[0].endereco_tabela;
    int n1_id_antigo = getIdNivel3Tabela1(endereco_antigo, offset, estado->bits_n1, estado->bits_n2, estado->bits_n3);
    int n2_id_antigo = getIdNivel3Tabela2(endereco_antigo, offset, estado->bits_n2, estado->bits_n3);
    int n3_id_antigo = getIdNivel3Tabela3(endereco_antigo, offset, estado->bits_n3);

    if(estado->tabela_pagina_n3[n1_id_antigo][n2_id_antigo][n3_id_antigo].sujo){
        estado->paginas_sujas++;
        estado->tabela_pagina_n3[n1_id_antigo][n2_id_antigo][n3_id_antigo].sujo = 0;
    }
    estado->tabela_pagina_n3[n1_id_antigo][n2_id_antigo][n3_id_antigo].valido = 0;

    for (int i = 0; i < estado->tam_memoria - 1; i++){
        estado->memoria[i] = estado->memoria[i + 1];
    }
    
    estado->memoria[estado->tam_memoria - 1].endereco_tabela = endereco;
    estado->memoria[estado->tam_memoria - 1].ultimo_acesso = estado->acessos_memoria;
}

void hierarquica3Random(estadoSimulacao *estado, int endereco, int offset){
    int num_random = rand() % estado->tam_memoria;
    int n1_id = getIdNivel3Tabela1(endereco, offset, estado->bits_n1, estado->bits_n2, estado->bits_n3);
    int n2_id = getIdNivel3Tabela2(endereco, offset, estado->bits_n2, estado->bits_n3);
    int n3_id = getIdNivel3Tabela3(endereco, offset, estado->bits_n3);
    
    estado->tabela_pagina_n3[n1_id][n2_id][n3_id].valido = 1;
    estado->tabela_pagina_n3[n1_id][n2_id][n3_id].endereco_memoria = num_random;

    int endereco_antigo = estado->memoria[num_random].endereco_tabela;
    int n1_id_antigo = getIdNivel3Tabela1(endereco_antigo, offset, estado->bits_n1, estado->bits_n2, estado->bits_n3);
    int n2_id_antigo = getIdNivel3Tabela2(endereco_antigo, offset, estado->bits_n2, estado->bits_n3);
    int n3_id_antigo = getIdNivel3Tabela3(endereco_antigo, offset, estado->bits_n3);

    if(estado->tabela_pagina_n3[n1_id_antigo][n2_id_antigo][n3_id_antigo].sujo){
        estado->paginas_sujas++;
        estado->tabela_pagina_n3[n1_id_antigo][n2_id_antigo][n3_id_antigo].sujo = 0;
    }
    estado->tabela_pagina_n3[n1_id_antigo][n2_id_antigo][n3_id_antigo].valido = 0;

    estado->memoria[num_random].endereco_tabela = endereco;
    estado->memoria[num_random].ultimo_acesso = estado->acessos_memoria;
}

void hierarquica3SegundaChance(estadoSimulacao *estado, int endereco, int offset){
    quadro *memoria = estado->memoria;
    lista *lista_circular = estado->lista_circular;
    node *indice = lista_circular->primeiro;

    while(memoria[indice->id].referencia == 1){
        memoria[indice->id].referencia = 0;
        indice = indice->proximo;
    }
    
    int n1_id = getIdNivel3Tabela1(endereco, offset, estado->bits_n1, estado->bits_n2, estado->bits_n3);
    int n2_id = getIdNivel3Tabela2(endereco, offset, estado->bits_n2, estado->bits_n3);
    int n3_id = getIdNivel3Tabela3(endereco, offset, estado->bits_n3);

    estado->tabela_pagina_n3[n1_id][n2_id][n3_id].valido = 1;
    estado->tabela_pagina_n3[n1_id][n2_id][n3_id].endereco_memoria = indice->id;

    int endereco_antigo = memoria[indice->id].endereco_tabela;
    int n1_id_antigo = getIdNivel3Tabela1(endereco_antigo, offset, estado->bits_n1, estado->bits_n2, estado->bits_n3);
    int n2_id_antigo = getIdNivel3Tabela2(endereco_antigo, offset, estado->bits_n2, estado->bits_n3);
    int n3_id_antigo = getIdNivel3Tabela3(endereco_antigo, offset, estado->bits_n3);

    if(estado->tabela_pagina_n3[n1_id_antigo][n2_id_antigo][n3_id_antigo].sujo){
        estado->paginas_sujas++;
        estado->tabela_pagina_n3[n1_id_antigo][n2_id_antigo][n3_id_antigo].sujo = 0;
    }
    estado->tabela_pagina_n3[n1_id_antigo][n2_id_antigo][n3_id_antigo].valido = 0;
    
    memoria[indice->id].endereco_tabela = endereco;
    memoria[indice->id].ultimo_acesso = estado->acessos_memoria;

    lista_circular->primeiro = indice->proximo;
    lista_circular->primeiro = indice;
}

void hierarquica3LRU(estadoSimulacao *estado, int endereco, int offset){
    quadro *memoria = estado->memoria;
    lista *lista_id = estado->lista_id;
    int id = lista_id->primeiro->id;
    int n1_id = getIdNivel3Tabela1(endereco, offset, estado->bits_n1, estado->bits_n2, estado->bits_n3);
    int n2_id = getIdNivel3Tabela2(endereco, offset, estado->bits_n2, estado->bits_n3);
    int n3_id = getIdNivel3Tabela3(endereco, offset, estado->bits_n3);
    
    estado->tabela_pagina_n3[n1_id][n2_id][n3_id].valido = 1;
    estado->tabela_pagina_n3[n1_id][n2_id][n3_id].endereco_memoria = id;

    int endereco_antigo = memoria[id].endereco_tabela;
    int n1_id_antigo = getIdNivel3Tabela1(endereco_antigo, offset, estado->bits_n1, estado->bits_n2, estado->bits_n3);
    int n2_id_antigo = getIdNivel3Tabela2(endereco_antigo, offset, estado->bits_n2, estado->bits_n3);
    int n3_id_antigo = getIdNivel3Tabela3(endereco_antigo, offset, estado->bits_n3);
    
    if(estado->tabela_pagina_n3[n1_id_antigo][n2_id_antigo][n3_id_antigo].sujo){
        estado->paginas_sujas++;
        estado->tabela_pagina_n3[n1_id_antigo][n2_id_antigo][n3_id_antigo].sujo = 0;
    }
    estado->tabela_pagina_n3[n1_id_antigo][n2_id_antigo][n3_id_antigo].valido = 0;

    memoria[id].endereco_tabela = endereco;
    memoria[id].ultimo_acesso = estado->acessos_memoria;

    node *no = lista_id->primeiro;
    lista_id->primeiro = lista_id->primeiro->proximo;
    lista_id->primeiro->anterior = NULL;
    no->proximo = NULL;
    no->anterior = lista_id->ultimo;
    lista_id->ultimo->proximo = no;
    lista_id->ultimo = no;
}