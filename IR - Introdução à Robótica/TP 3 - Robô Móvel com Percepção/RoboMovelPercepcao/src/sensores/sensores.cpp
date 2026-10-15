#include "sensores.h"
#include "config.h"
#include <Wire.h>
#include <math.h>

// Integracao de 50 ms: Menos ruido
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

int leituraDistAtual = -1;
int corVistaAtual    = -1;
int corDetectada     = -1;

// Calibracao padrao: [cor][rn,gn,bn]
float refCor[4][3] = {
  { 0.55, 0.27, 0.18 },    // VERMELHO
  { 0.27, 0.50, 0.23 },    // VERDE
  { 0.22, 0.33, 0.45 },    // AZUL
  { 0.333, 0.333, 0.334 }  // BRANCO / PAREDE
};

bool iniciaSensores() {
  return tcs.begin();
}

float leDistanciaCm() {
  digitalWrite(PIN_TRIG, LOW);  delayMicroseconds(3);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  unsigned long dur = pulseIn(PIN_ECHO, HIGH, 25000UL);  // timeout ~4 m
  if (dur == 0) return -1.0;
  return dur * 0.0343 / 2.0;
}

// Filtro de media movel - Retorna cromaticidade normalizada pela intensidade
// Estamos usando n=5 na calibracao e n=2 quando se move
void leCromaticidade(float &rn, float &gn, float &bn, uint16_t &clear, uint8_t n) {
  if (n < 1) n = 1;
  unsigned long sr = 0, sg = 0, sb = 0, sc = 0;
  for (uint8_t i = 0; i < n; i++) {
    uint16_t r, g, b, c;
    tcs.getRawData(&r, &g, &b, &c);
    sr += r; sg += g; sb += b; sc += c;
  }
  clear = (uint16_t)(sc / n);
  float soma = (float)sr + (float)sg + (float)sb;
  if (soma < 1.0) soma = 1.0;
  rn = sr / soma;
  gn = sg / soma;
  bn = sb / soma;
}

// Classifica cor: 0,1,2,3 = bloco, 4 = branco/parede,
// -1 = escuro ou cor nao reconhecida
int classificaCromaticidade(float rn, float gn, float bn, uint16_t clear) {
  if (clear < CLEAR_MIN) return -1;             // escuro / sem objeto

  // 1) Branco
  float dwr = rn - refCor[COR_BRANCO][0];
  float dwg = gn - refCor[COR_BRANCO][1];
  float dwb = bn - refCor[COR_BRANCO][2];
  float distBranco = sqrt(dwr*dwr + dwg*dwg + dwb*dwb);
  if (distBranco < DIST_MAX_BRANCO) return COR_BRANCO;

  // 2) Blocos - Calcula distancia para as cores calibradas e retorna a menor
  int idx = 0;
  float menor = 1e9;
  for (int i = 0; i < 3; i++) {
    float dr = rn - refCor[i][0];
    float dg = gn - refCor[i][1];
    float db = bn - refCor[i][2];
    float dd = sqrt(dr*dr + dg*dg + db*db);
    if (dd < menor) { menor = dd; idx = i; }
  }
  if (menor > DIST_MAX) return -1;              // nao identificou
  return idx;
}

// Le o sensor RGB e classifica a cor
int classificaCor(uint8_t n) {
  float rn, gn, bn;
  uint16_t c;
  leCromaticidade(rn, gn, bn, c, n);
  return classificaCromaticidade(rn, gn, bn, c);
}

// monitora durante o movimento: 0 nada, 1 parede, 2 bloco (corDetectada)
uint8_t monitoraSensores() {
  static unsigned long ultimo = 0;
  if (millis() - ultimo < 80) return 0; // So le a cada 80 ms
  ultimo = millis();

  float d = leDistanciaCm();
  leituraDistAtual = (d < 0) ? -1 : (int)d;

  if (d < 0 || d > DIST_DETECTA) { corVistaAtual = -1; return 0; }

  if (d <= DIST_COR_MAX) { // So olha pra cor se objeto ta perto
    int c = classificaCor(2);
    corVistaAtual = c;
    if (c >= 0 && c <= 2) { corDetectada = c; return 2; } // Retorna bloco
  } else {
    corVistaAtual = -1;
  }
  if (d <= DIST_PAREDE) return 1;               // parede
  return 0;                                     // aproximando
}

// LDRs de linha: true quando o pino le o nivel definido como preto
bool linhaEsqPreto() { return digitalRead(PIN_LDR_ESQ) == LINHA_NIVEL_PRETO; }
bool linhaDirPreto() { return digitalRead(PIN_LDR_DIR) == LINHA_NIVEL_PRETO; }

// LDRs de alinhamento: valor analogico bruto 
int leLdrTesteEsq() { return analogRead(PIN_LDR_TESTE_ESQ); }
int leLdrTesteDir() { return analogRead(PIN_LDR_TESTE_DIR); }

// LDR inferior: true quando o pino le o nivel definido como luz acesa
bool luzAcesa() { return digitalRead(PIN_LDR_BAIXO) == LUZ_NIVEL_ACESO; }
