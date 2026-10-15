#include "encoders.h"
#include "config.h"

volatile long encEsq = 0;
volatile long encDir = 0;
volatile unsigned long tUltEsq = 0;
volatile unsigned long tUltDir = 0;

// Atualiza contagem encoder
void isrEsq() {
  unsigned long agora = micros();
  if (agora - tUltEsq < DEBOUNCE_US) return;   // reduzir ruido
  tUltEsq = agora;
  encEsq++;
}
void isrDir() {
  unsigned long agora = micros();
  if (agora - tUltDir < DEBOUNCE_US) return;
  tUltDir = agora;
  encDir++;
}

void zeraEncoders() {
  noInterrupts();
  encEsq = 0;
  encDir = 0;
  interrupts();
}
long lerEncEsq() { noInterrupts(); long v = encEsq; interrupts(); return v; }
long lerEncDir() { noInterrupts(); long v = encDir; interrupts(); return v; }
