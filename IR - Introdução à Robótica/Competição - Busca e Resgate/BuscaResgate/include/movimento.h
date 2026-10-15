#pragma once
/*
  movimento.h - Movimento baseado nos encoders
*/

#include <Arduino.h>

extern uint8_t movKind;
extern long    movAlvo;
extern bool    movAtivo;
extern int     velGiroAtual;   // velocidade usada nos giros
extern int     velFrenteAtual; // velocidade base do avanco (cai ao se aproximar)
extern unsigned long tParou;   // instante em que o ultimo movimento parou

void iniciaMovimento(uint8_t kind, long alvo);
bool atualizaMovimento();
