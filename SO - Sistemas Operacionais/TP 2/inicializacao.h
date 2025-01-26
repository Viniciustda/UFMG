#ifndef INICIALIZACAO_H
#define INICIALIZACAO_H

#include "structs.h"

// -------------------------------------------------- Funções de Inicialização --------------------------------------------------

void iniciaEstadoSimulacao(estadoSimulacao *estado, int tam_total_mem, int tam_pagina, int num_bits_endereco);

void inicializaTabelaPaginas(estadoSimulacao *estado);

void inicializaTabelaPaginasInvertida(estadoSimulacao *estado);

void inicializaN1TabelaN2(estadoSimulacao *estado);

void inicializaN2TabelaN2(estadoSimulacao *estado, int id);

void inicializaN1TabelaN3(estadoSimulacao *estado);

void inicializaN2TabelaN3(estadoSimulacao *estado, int n1_id);

void inicializaN3TabelaN3(estadoSimulacao *estado, int n1_id, int n2_id);

void inicializaMemoria(estadoSimulacao *estado);

void inicializaListaAux(lista **lista);

void liberaEstadoSimulacao(estadoSimulacao *estado);

#endif