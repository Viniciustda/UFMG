#pragma once
/*
  encoders.h - Encoders LM393 (disco 20 furos)
*/

#include <Arduino.h>

extern volatile long encEsq;
extern volatile long encDir;
extern volatile unsigned long tUltEsq;  // tempo do ultimo pulso no esquerdo
extern volatile unsigned long tUltDir;  // tempo do ultimo pulso no direito

void isrEsq();
void isrDir();
void zeraEncoders();
long lerEncEsq();
long lerEncDir();
