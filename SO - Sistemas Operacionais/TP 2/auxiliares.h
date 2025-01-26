#ifndef AUXILIARES_H
#define AUXILIARES_H

#include <stdio.h>
#include <string.h>

void printParametros();

int potenciaDeDois(int x);

int validaArgumentos(int argc, char *algoritmo, FILE *arquivo_entrada, int tam_pagina, int tam_total_mem, char *tp_tabela);

int determinaS(int tam_pagina);

void geraRelatorio(char *arquivo_entrada, int tam_total_mem, int tam_pagina, char *algoritmo, char *tp_tabela, int acessos_memoria, int page_faults, int acessos_mem_R, int acessos_mem_W, int paginas_sujas);

#endif