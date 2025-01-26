#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "structs.h"

void densaFIFO(estadoSimulacao *estado, int endereco);
void densaRandom(estadoSimulacao *estado, int endereco);
void densaSegundaChance(estadoSimulacao *estado, int endereco);
void densaLRU(estadoSimulacao *estado, int endereco);

int invertidaFIFO(estadoSimulacao *estado, int endereco);
int invertidaRandom(estadoSimulacao *estado, int endereco);
int invertidaSegundaChance(estadoSimulacao *estado, int endereco);
int invertidaLRU(estadoSimulacao *estado, int endereco);
int buscaTabelaInvertida(estadoSimulacao *estado, int endereco_tabela);

int getIdNivel2Tabela1(unsigned endereco, int offset, int bits_n1, int bits_n2);
int getIdNivel2Tabela2(unsigned endereco, int offset, int bits_n2);
int getIdNivel3Tabela1(unsigned endereco, int offset, int bits_n1, int bits_n2, int bits_n3);
int getIdNivel3Tabela2(unsigned endereco, int offset, int bits_n2, int bits_n3);
int getIdNivel3Tabela3(unsigned endereco, int offset, int bits_n3);

void hierarquica2FIFO(estadoSimulacao *estado, int endereco, int offset);
void hierarquica2Random(estadoSimulacao *estado, int endereco, int offset);
void hierarquica2SegundaChance(estadoSimulacao *estado, int endereco, int offset);
void hierarquica2LRU(estadoSimulacao *estado, int endereco, int offset);

void hierarquica3FIFO(estadoSimulacao *estado, int endereco, int offset);
void hierarquica3Random(estadoSimulacao *estado, int endereco, int offset);
void hierarquica3SegundaChance(estadoSimulacao *estado, int endereco, int offset);
void hierarquica3LRU(estadoSimulacao *estado, int endereco, int offset);

#endif