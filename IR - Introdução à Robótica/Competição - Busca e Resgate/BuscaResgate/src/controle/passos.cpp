#include "passos.h"
#include "config.h"
#include "movimento.h"

uint8_t passoTipo[MAX_PASSOS];
float   passoValor[MAX_PASSOS];
int8_t  passoDir[MAX_PASSOS];
uint8_t numPassos   = 0;
uint8_t passoAtual  = 0;

void addPasso(uint8_t tipo, float val, int8_t dir) {
  if (numPassos >= MAX_PASSOS) return;
  passoTipo[numPassos]  = tipo;
  passoValor[numPassos] = val;
  passoDir[numPassos]   = dir;
  numPassos++;
}

void iniciaPasso(uint8_t i) {
  if (passoTipo[i] == ST_FRENTE)
    iniciaMovimento(MK_FRENTE, (long)(passoValor[i] * COUNTS_PER_CM) - FOLGA_RETA);
  else if (passoTipo[i] == ST_RE)
    iniciaMovimento(MK_RE, (long)(passoValor[i] * COUNTS_PER_CM) - FOLGA_RETA);
  else {
    velGiroAtual = VEL_GIRO;   // trajetos giram na velocidade normal
    iniciaMovimento(passoDir[i] > 0 ? MK_GIRA_DIR : MK_GIRA_ESQ, (long)(passoValor[i] * COUNTS_PER_DEG) - FOLGA_GIRO);
  }
}

static void montaPassosTrajeto(uint8_t tipo, float tamanho) {
  if (tipo == TRAJ_RETA) {             // Reta: ida e volta
    addPasso(ST_FRENTE, tamanho, 0);
    addPasso(ST_GIRA,   180,    +1);
    addPasso(ST_FRENTE, tamanho, 0);
    addPasso(ST_GIRA,   180,    +1);
  } else {                             // Quadrado
    for (uint8_t k = 0; k < 4; k++) {
      addPasso(ST_FRENTE, tamanho, 0);
      addPasso(ST_GIRA,   90,     +1);
    }
  }
}

void carregaTrajeto(uint8_t tipo, float tamanho) {
  numPassos = 0;
  montaPassosTrajeto(tipo, tamanho);
  passoAtual = 0;
  iniciaPasso(0);
}

// Apos detectar um bloco: gira p/ direita (180 na reta, 90 no quadrado) e recomeca o trajeto do inicio
void reiniciaTrajetoAposBloco(uint8_t tipo, float tamanho) {
  numPassos = 0;
  float angulo = (tipo == TRAJ_RETA) ? 180.0 : 90.0;
  addPasso(ST_GIRA, angulo, +1);       // +1 = direita
  montaPassosTrajeto(tipo, tamanho);
  passoAtual = 0;
  iniciaPasso(0);
}
