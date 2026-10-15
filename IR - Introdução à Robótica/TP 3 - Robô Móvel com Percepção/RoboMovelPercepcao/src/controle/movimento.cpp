#include "movimento.h"
#include "config.h"
#include "motores.h"
#include "encoders.h"

uint8_t movKind  = MK_FRENTE;
long    movAlvo  = 0;
bool    movAtivo = false;
int     velGiroAtual = VEL_GIRO;
int     velFrenteAtual = VEL_FRENTE;   // vel padrao; cai p/ VEL_LENTA ao se aproximar
unsigned long tParou = 0;
long    erroAnt  = 0;   // erro (e-d) da iteracao anterior, para o termo derivativo

void iniciaMovimento(uint8_t kind, long alvo) {
  zeraEncoders();
  erroAnt  = 0;
  velFrenteAtual = VEL_FRENTE;
  movKind  = kind;
  movAlvo  = (alvo < 1) ? 1 : alvo;
  movAtivo = true;
}

bool atualizaMovimento() {
  if (!movAtivo) return false;

  long e = lerEncEsq();
  long d = lerEncDir();
  long progresso = (e + d) / 2;

  if (progresso >= movAlvo) {
    paraMotores();
    movAtivo = false;
    tParou = millis();                       // pausa entre passos
    Serial.print(F("Movimento concluido. encEsq="));
    Serial.print(e); Serial.print(F(" encDir=")); Serial.println(d);
    return false;
  }

  if (movKind == MK_FRENTE) {
    long erro = e - d;
    int corr = (int)(KP_RETO * erro + KD_RETO * (erro - erroAnt));  // Controlador PD
    erroAnt = erro;
    int vE = velFrenteAtual - corr;
    int vD = velFrenteAtual + corr;
    if (vE < VEL_MIN) vE = VEL_MIN;
    if (vD < VEL_MIN) vD = VEL_MIN;
    if (vE > 255)     vE = 255;
    if (vD > 255)     vD = 255;
    setMotor(0, vE);
    setMotor(1, vD);
  } else if (movKind == MK_RE) {
    setMotor(0, -VEL_RE);
    setMotor(1, -VEL_RE);
  } else if (movKind == MK_GIRA_DIR) {
    setMotor(0,  velGiroAtual);
    setMotor(1, -velGiroAtual);
  } else {
    setMotor(0, -velGiroAtual);
    setMotor(1,  velGiroAtual);
  }
  return true;
}
