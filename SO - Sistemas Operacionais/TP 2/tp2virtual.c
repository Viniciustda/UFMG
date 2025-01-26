#include <stdio.h>
#include <stdlib.h>
#include "inicializacao.h"
#include "auxiliares.h"
#include "simulacao.h"

// -------------------------------------------------- Main --------------------------------------------------

int main(int argc, char* argv[]){

    char *algoritmo = argv[1];
    FILE *arquivo;
    arquivo = fopen(argv[2], "r"); 
    int tam_pagina = atoi(argv[3]);
    int tam_total_mem = atoi(argv[4]);
    char *tp_tabela = argv[5];
    if(argv[5] == NULL)
        tp_tabela = "densa";

    if (validaArgumentos(argc, algoritmo, arquivo, tam_pagina, tam_total_mem, tp_tabela)) {
        return 1;
    }

    int s = determinaS(tam_pagina);
    int num_bits_endereco = 32 - s;

    estadoSimulacao estado;
    iniciaEstadoSimulacao(&estado, tam_total_mem, tam_pagina, num_bits_endereco);
    
    printf("\n=============================================\n");
    printf("\nExecutando o simulador...\n");

    if(strcmp(tp_tabela, "densa") == 0){ 
        memoriaVirtual(&estado, arquivo, s, algoritmo);
    }
    else if(strcmp(tp_tabela, "hierarquica2") == 0){
        memoriaVirtualHierarquica2(&estado, arquivo, s, algoritmo);
    }
    else if(strcmp(tp_tabela, "hhierarquica3") == 0){
        memoriaVirtualHierarquica3(&estado, arquivo, s, algoritmo);
    }
    else if(strcmp(tp_tabela, "inversa") == 0){
        memoriaVirtualInvertida(&estado, arquivo, s, algoritmo);
    }

    geraRelatorio(argv[2], tam_total_mem, tam_pagina, algoritmo, tp_tabela, estado.acessos_memoria, estado.pageFaults, estado.acessos_mem_R, estado.acessos_mem_W, estado.paginas_sujas);

    liberaEstadoSimulacao(&estado);

    fclose(arquivo);

    return 0;
}