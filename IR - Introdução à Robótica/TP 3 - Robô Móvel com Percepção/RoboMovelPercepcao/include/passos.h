#pragma once
/*
  passos.h - Trajetos são listas de passos
*/

#include <Arduino.h>
#include "config.h"

extern uint8_t passoTipo[MAX_PASSOS];
extern float   passoValor[MAX_PASSOS];
extern int8_t  passoDir[MAX_PASSOS];   // +1 direita, -1 esquerda
extern uint8_t numPassos;
extern uint8_t passoAtual;

void addPasso(uint8_t tipo, float val, int8_t dir);
void iniciaPasso(uint8_t i);
void carregaTrajeto(uint8_t tipo, float tamanho);   // TRAJ_RETA ou TRAJ_QUADRADO
void reiniciaTrajetoAposBloco(uint8_t tipo, float tamanho);  // desvia e recomeca
