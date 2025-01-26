#include "simulacao.h"
#include "inicializacao.h"
#include "listas.h"
#include "algoritmos.h"
#include "structs.h"

// -------------------------------------------------- Funções de Simulação de Memória Virtual --------------------------------------------------

void memoriaVirtual(estadoSimulacao *estado, FILE *arquivo, int offset, char *algoritmo){
    unsigned endereco;
    char rw;
    while(fscanf(arquivo,"%x %c",&endereco,&rw) != -1){
        estado->acessos_memoria++;
        if(tolower(rw) == 'w')
            estado->acessos_mem_W++;
        else
            estado->acessos_mem_R++;

        int endereco_tabela = endereco >> offset;
        if(estado->tabela_paginas[endereco_tabela].valido == 1){
            if(tolower(rw) == 'w'){
                estado->tabela_paginas[endereco_tabela].sujo = 1; 
                estado->memoria[estado->tabela_paginas[endereco_tabela].endereco_memoria].ultimo_acesso = estado->acessos_memoria;
            }
            else{
                if(estado->tabela_paginas[endereco_tabela].sujo)
                    estado->tabela_paginas[endereco_tabela].sujo = 1;
                else
                    estado->tabela_paginas[endereco_tabela].sujo = 0;
            }

            if(strcmp(algoritmo, "lru") == 0)
                atualizaLista(estado, estado->tabela_paginas[endereco_tabela].endereco_memoria);
        }
        else {
            estado->pageFaults++;

            if(estado->tam_memoria > estado->atual_tam_memoria){
                estado->tabela_paginas[endereco_tabela].valido = 1;
                estado->tabela_paginas[endereco_tabela].endereco_memoria = estado->atual_tam_memoria;

                estado->memoria[estado->atual_tam_memoria].endereco_tabela = endereco_tabela;
                estado->memoria[estado->atual_tam_memoria].ultimo_acesso = estado->acessos_memoria;

                if(strcmp(algoritmo, "lru") == 0)
                    adicionaLista(estado);
                if(strcmp(algoritmo, "secondChance") == 0)
                    adicionaListaCircular(estado);
                
                estado->atual_tam_memoria++;  
            }
            else {
                estado->acessos_disco++;
                if(strcmp(algoritmo, "fifo") == 0){
                    densaFIFO(estado, endereco_tabela);
                }
                else if(strcmp(algoritmo, "lru") == 0){
                    densaLRU(estado, endereco_tabela);
                }
                else if(strcmp(algoritmo, "secondChance") == 0){
                    densaSegundaChance(estado, endereco_tabela);
                }
                else if(strcmp(algoritmo, "random") == 0){
                    densaRandom(estado, endereco_tabela);
                }
            }

            if(tolower(rw) == 'w'){
                estado->tabela_paginas[endereco_tabela].sujo = 1; 
                estado->memoria[estado->tabela_paginas[endereco_tabela].endereco_memoria].ultimo_acesso = estado->acessos_memoria;
            }
            else{
                if(estado->tabela_paginas[endereco_tabela].sujo)
                    estado->tabela_paginas[endereco_tabela].sujo = 1;
                else
                    estado->tabela_paginas[endereco_tabela].sujo = 0;
            }
        }
        estado->memoria[estado->tabela_paginas[endereco_tabela].endereco_memoria].referencia = 1;
    }
}

void memoriaVirtualInvertida(estadoSimulacao *estado, FILE *arquivo, int offset, char *algoritmo){
    unsigned endereco;
    char rw;
    while(fscanf(arquivo,"%x %c",&endereco,&rw) != -1){

        estado->acessos_memoria++;
        if(tolower(rw) == 'w')
            estado->acessos_mem_W++;
        else
            estado->acessos_mem_R++;

        int endereco_tabela = endereco >> offset;
        int tablePosition = buscaTabelaInvertida(estado, endereco_tabela);

        if(tablePosition >= 0){

            if(tolower(rw) == 'w'){
                estado->tabela_paginas_invertida[tablePosition].sujo = 1; 
                estado->memoria[estado->tabela_paginas_invertida[tablePosition].endereco_memoria].ultimo_acesso = estado->acessos_memoria;
            }
            else{
                if(estado->tabela_paginas_invertida[tablePosition].sujo)
                    estado->tabela_paginas_invertida[tablePosition].sujo = 1;
                else
                    estado->tabela_paginas_invertida[tablePosition].sujo = 0;
            }

            if(strcmp(algoritmo, "lru") == 0)
                atualizaLista(estado, estado->tabela_paginas_invertida[tablePosition].endereco_memoria); 
        }
        else {
            estado->pageFaults++;
            if(estado->tam_memoria > estado->atual_tam_memoria){              

                estado->tabela_paginas_invertida[estado->atual_tam_memoria].endereco_virtual = endereco_tabela;
                estado->tabela_paginas_invertida[estado->atual_tam_memoria].endereco_memoria = estado->atual_tam_memoria;

                estado->memoria[estado->atual_tam_memoria].endereco_tabela = estado->atual_tam_memoria;
                estado->memoria[estado->atual_tam_memoria].ultimo_acesso = estado->acessos_memoria;  

                if(strcmp(algoritmo, "lru") == 0)
                    adicionaLista(estado);
                if(strcmp(algoritmo, "secondChance") == 0)
                    adicionaListaCircular(estado);

                tablePosition = estado->atual_tam_memoria;

                estado->atual_tam_memoria++;  
            }
            else {
                estado->acessos_disco++;
                if(strcmp(algoritmo, "fifo") == 0){
                    tablePosition = invertidaFIFO(estado, endereco_tabela);
                }
                else if(strcmp(algoritmo, "lru") == 0){
                    tablePosition = invertidaLRU(estado, endereco_tabela);
                }
                else if(strcmp(algoritmo, "secondChance") == 0){
                    tablePosition = invertidaSegundaChance(estado, endereco_tabela);
                }
                else if(strcmp(algoritmo, "random") == 0){
                    tablePosition = invertidaRandom(estado, endereco_tabela);
                }
            }
            
            if(tolower(rw) == 'w'){
                estado->tabela_paginas_invertida[tablePosition].sujo = 1; 
                estado->memoria[estado->tabela_paginas_invertida[tablePosition].endereco_memoria].ultimo_acesso = estado->acessos_memoria;
            }
            else{
                if(estado->tabela_paginas_invertida[tablePosition].sujo)
                    estado->tabela_paginas_invertida[tablePosition].sujo = 1;
                else
                    estado->tabela_paginas_invertida[tablePosition].sujo = 0;
            }
            
        }
        
        estado->memoria[estado->tabela_paginas_invertida[tablePosition].endereco_memoria].referencia = 1;
    }
}

void memoriaVirtualHierarquica2(estadoSimulacao *estado, FILE *arquivo, int offset, char *algoritmo){
    unsigned endereco;
    char rw;

    while(fscanf(arquivo,"%x %c",&endereco,&rw) != -1){
        
        estado->acessos_memoria++;
        if(tolower(rw) == 'w')
            estado->acessos_mem_W++;
        else
            estado->acessos_mem_R++;

        int n1_id = getIdNivel2Tabela1(endereco, offset, estado->bits_n1, estado->bits_n2);
        int n2_id = getIdNivel2Tabela2(endereco, offset, estado->bits_n2);

        if (estado->tabela_pagina_n2[n1_id] == NULL) {
            inicializaN2TabelaN2(estado, n1_id);
        }

        if(estado->tabela_pagina_n2[n1_id][n2_id].valido){
            if(tolower(rw) == 'w'){
                estado->tabela_pagina_n2[n1_id][n2_id].sujo = 1; 
                estado->memoria[estado->tabela_pagina_n2[n1_id][n2_id].endereco_memoria].ultimo_acesso = estado->acessos_memoria;
            }
            else{
                if(estado->tabela_pagina_n2[n1_id][n2_id].sujo)
                    estado->tabela_pagina_n2[n1_id][n2_id].sujo = 1;
                else
                    estado->tabela_pagina_n2[n1_id][n2_id].sujo = 0;
            }
            if(strcmp(algoritmo, "lru") == 0)
                atualizaLista(estado, estado->tabela_pagina_n2[n1_id][n2_id].endereco_memoria);
        }
        else {
            estado->pageFaults++;
            if(estado->tam_memoria > estado->atual_tam_memoria){
                estado->tabela_pagina_n2[n1_id][n2_id].valido = 1;
                estado->tabela_pagina_n2[n1_id][n2_id].endereco_memoria = estado->atual_tam_memoria;

                estado->memoria[estado->atual_tam_memoria].endereco_tabela = endereco;
                estado->memoria[estado->atual_tam_memoria].ultimo_acesso = estado->acessos_memoria;             

                if(strcmp(algoritmo, "lru") == 0)
                    adicionaLista(estado);
                if(strcmp(algoritmo, "secondChance") == 0)
                    adicionaListaCircular(estado);

                estado->atual_tam_memoria++;  
            }
            else {
                estado->acessos_disco++;
                if(strcmp(algoritmo, "fifo") == 0){
                    hierarquica2FIFO(estado, endereco, offset);
                }
                else if(strcmp(algoritmo, "lru") == 0){
                    hierarquica2LRU(estado, endereco, offset);
                }
                else if(strcmp(algoritmo, "secondChance") == 0){
                    hierarquica2SegundaChance(estado, endereco, offset);
                }
                else if(strcmp(algoritmo, "random") == 0){
                    hierarquica2Random(estado, endereco, offset);
                }
            }

            if(tolower(rw) == 'w'){
                estado->tabela_pagina_n2[n1_id][n2_id].sujo = 1; 
                estado->memoria[estado->tabela_pagina_n2[n1_id][n2_id].endereco_memoria].ultimo_acesso = estado->acessos_memoria;
            }
            else{
                if(estado->tabela_pagina_n2[n1_id][n2_id].sujo)
                    estado->tabela_pagina_n2[n1_id][n2_id].sujo = 1;
                else
                    estado->tabela_pagina_n2[n1_id][n2_id].sujo = 0;
            }
        }
        estado->memoria[estado->tabela_pagina_n2[n1_id][n2_id].endereco_memoria].referencia = 1;
    }
}

void memoriaVirtualHierarquica3(estadoSimulacao *estado, FILE *arquivo, int offset, char *algoritmo) {
    unsigned endereco;
    char rw;

    while (fscanf(arquivo, "%x %c", &endereco, &rw) != -1) {
        
        estado->acessos_memoria++;
        if(tolower(rw) == 'w')
            estado->acessos_mem_W++;
        else
            estado->acessos_mem_R++;

        int n1_id = getIdNivel3Tabela1(endereco, offset, estado->bits_n1, estado->bits_n2, estado->bits_n3);
        int n2_id = getIdNivel3Tabela2(endereco, offset, estado->bits_n2, estado->bits_n3);
        int n3_id = getIdNivel3Tabela3(endereco, offset, estado->bits_n3);

        if (estado->tabela_pagina_n3[n1_id] == NULL) {
            inicializaN2TabelaN3(estado, n1_id);
        }

        if (estado->tabela_pagina_n3[n1_id][n2_id] == NULL) {
            inicializaN3TabelaN3(estado, n1_id, n2_id);
        }

        if (estado->tabela_pagina_n3[n1_id][n2_id][n3_id].valido) {
            if(tolower(rw) == 'w'){
                estado->tabela_pagina_n3[n1_id][n2_id][n3_id].sujo = 1; 
                estado->memoria[estado->tabela_pagina_n3[n1_id][n2_id][n3_id].endereco_memoria].ultimo_acesso = estado->acessos_memoria;
            }
            else{
                if(estado->tabela_pagina_n3[n1_id][n2_id][n3_id].sujo)
                    estado->tabela_pagina_n3[n1_id][n2_id][n3_id].sujo = 1;
                else
                    estado->tabela_pagina_n3[n1_id][n2_id][n3_id].sujo = 0;
            }

            if (strcmp(algoritmo, "lru") == 0) {
                atualizaLista(estado, estado->tabela_pagina_n3[n1_id][n2_id][n3_id].endereco_memoria);
            }
        } else {
            estado->pageFaults++;
            if (estado->tam_memoria > estado->atual_tam_memoria) {
                estado->tabela_pagina_n3[n1_id][n2_id][n3_id].valido = 1;
                estado->tabela_pagina_n3[n1_id][n2_id][n3_id].endereco_memoria = estado->atual_tam_memoria;

                estado->memoria[estado->atual_tam_memoria].endereco_tabela = endereco;
                estado->memoria[estado->atual_tam_memoria].ultimo_acesso = estado->acessos_memoria;

                if (strcmp(algoritmo, "lru") == 0) 
                    adicionaLista(estado);
                if(strcmp(algoritmo, "secondChance") == 0)
                    adicionaListaCircular(estado);

                estado->atual_tam_memoria++;
            } else {
                estado->acessos_disco++;
                if (strcmp(algoritmo, "fifo") == 0) {
                    hierarquica3FIFO(estado, endereco, offset);
                } else if (strcmp(algoritmo, "lru") == 0) {
                    hierarquica3LRU(estado, endereco, offset);
                } else if (strcmp(algoritmo, "secondChance") == 0) {
                    hierarquica3SegundaChance(estado, endereco, offset);
                } else if (strcmp(algoritmo, "random") == 0) {
                    hierarquica3Random(estado, endereco, offset);
                }
            }

            if(tolower(rw) == 'w'){
                estado->tabela_pagina_n3[n1_id][n2_id][n3_id].sujo = 1; 
                estado->memoria[estado->tabela_pagina_n3[n1_id][n2_id][n3_id].endereco_memoria].ultimo_acesso = estado->acessos_memoria;
            }
            else{
                if(estado->tabela_pagina_n3[n1_id][n2_id][n3_id].sujo)
                    estado->tabela_pagina_n3[n1_id][n2_id][n3_id].sujo = 1;
                else
                    estado->tabela_pagina_n3[n1_id][n2_id][n3_id].sujo = 0;
            }
        }
        estado->memoria[estado->tabela_pagina_n3[n1_id][n2_id][n3_id].endereco_memoria].referencia = 1;
    }
}