#ifndef SIMULACAO_H
#define SIMULACAO_H

#include "inicializacao.h"
#include "listas.h"
#include "algoritmos.h"
#include "structs.h"

void memoriaVirtual(estadoSimulacao *estado, FILE *arquivo, int offset, char *algoritmo);
void memoriaVirtualInvertida(estadoSimulacao *estado, FILE *arquivo, int offset, char *algoritmo);
void memoriaVirtualHierarquica2(estadoSimulacao *estado, FILE *arquivo, int offset, char *algoritmo);
void memoriaVirtualHierarquica3(estadoSimulacao *estado, FILE *arquivo, int offset, char *algoritmo);

#endif