/*
  =====================================================================
  Teste de sensores: COR (TCS34725) + DISTANCIA (HC-SR04)
  Placa: Arduino Mega 2560
  ---------------------------------------------------------------------
  Ligacoes:
    TCS34725 : VIN->5V   GND->GND   SDA->20   SCL->21
    HC-SR04  : VCC->5V   GND->GND   Trig->41  Echo->43
  ---------------------------------------------------------------------
  Cores reconhecidas: VERMELHO, VERDE, AZUL, AMARELO
  ---------------------------------------------------------------------
  Abra o Monitor Serial em 115200 baud.

  CALIBRACAO (recomendada) - aponte o sensor para cada cor, a uns
  1-2 cm de distancia, e envie pela Serial a tecla correspondente:
      r  -> salva a referencia do VERMELHO
      g  -> salva a referencia do VERDE
      b  -> salva a referencia do AZUL
      y  -> salva a referencia do AMARELO
      p  -> mostra as referencias atuais
  As referencias ficam na memoria ate desligar/resetar o Arduino.
  =====================================================================
*/

#include <Wire.h>
#include "Adafruit_TCS34725.h"

// ----------------------- Configuracoes -------------------------------
const uint8_t TRIG_PIN = 41;
const uint8_t ECHO_PIN = 43;

// Sensor de cor. Se o canal "clear" ficar perto de 65535 (saturado),
// troque TCS34725_GAIN_4X por TCS34725_GAIN_1X.
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS,
                                          TCS34725_GAIN_4X);

// Limiares - ajuste observando os numeros impressos no Serial.
const uint16_t CLEAR_MIN = 60;     // abaixo disso: escuro / sem objeto
const float    SAT_MIN   = 0.045;  // abaixo disso: superficie branca/cinza
const float    DIST_MAX  = 0.20;   // acima disso: cor nao reconhecida

// ----------------------- Referencias de cor --------------------------
// Cada cor e guardada como cromaticidade [rn, gn, bn] (somam ~1), o que
// deixa a leitura quase independente do brilho. Os valores abaixo sao
// aproximados: CALIBRE pela Serial para ter boa precisao.
const char* nomeCor[4] = { "VERMELHO", "VERDE", "AZUL", "AMARELO" };
float refCor[4][3] = {
  { 0.55, 0.27, 0.18 },   // VERMELHO
  { 0.27, 0.50, 0.23 },   // VERDE
  { 0.22, 0.33, 0.45 },   // AZUL
  { 0.45, 0.42, 0.13 }    // AMARELO
};

// =====================================================================
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  Wire.begin();
  if (!tcs.begin()) {
    Serial.println(F("ERRO: TCS34725 nao encontrado. Cheque SDA(20)/SCL(21)/5V/GND."));
    while (1) delay(10);
  }

  Serial.println(F("=== Teste TCS34725 + HC-SR04 ==="));
  Serial.println(F("Calibre enviando r/g/b/y apontando cada cor. 'p' mostra as referencias."));
  Serial.println();
}

// ------------------- Leitura do sensor de cor ------------------------
// Devolve a cromaticidade (rn, gn, bn) e o canal clear.
void lerCor(float &rn, float &gn, float &bn, uint16_t &clear) {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);   // ja aguarda o tempo de integracao
  clear = c;
  float soma = (float)r + (float)g + (float)b;
  if (soma < 1.0) soma = 1.0;       // evita divisao por zero
  rn = r / soma;
  gn = g / soma;
  bn = b / soma;
}

// Indice da referencia mais proxima da cor lida (e a distancia ate ela).
int idxMaisProximo(float rn, float gn, float bn, float &menorDist) {
  int idx = 0;
  menorDist = 1e9;
  for (int i = 0; i < 4; i++) {
    float dr = rn - refCor[i][0];
    float dg = gn - refCor[i][1];
    float db = bn - refCor[i][2];
    float d  = sqrt(dr * dr + dg * dg + db * db);
    if (d < menorDist) { menorDist = d; idx = i; }
  }
  return idx;
}

// Classifica a cor lida.
const char* classificarCor(float rn, float gn, float bn,
                           uint16_t clear, float &distRef) {
  distRef = 0;
  if (clear < CLEAR_MIN) return "ESCURO/SEM OBJETO";

  // saturacao = distancia ate o ponto neutro (cinza)
  const float n = 0.3333;
  float sat = sqrt((rn - n) * (rn - n) +
                   (gn - n) * (gn - n) +
                   (bn - n) * (bn - n));
  if (sat < SAT_MIN) return "BRANCO/CINZA";

  int idx = idxMaisProximo(rn, gn, bn, distRef);
  if (distRef > DIST_MAX) return "INDEFINIDA";
  return nomeCor[idx];
}

// ------------------- Leitura do HC-SR04 ------------------------------
float umaLeituraCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long dur = pulseIn(ECHO_PIN, HIGH, 25000UL); // timeout ~4 m
  if (dur == 0) return -1.0;                            // nada detectado
  return dur * 0.0343 / 2.0;                            // som a 343 m/s
}

// Mediana de 3 leituras, para reduzir ruido do ultrassom.
float lerDistanciaCm() {
  float v[3];
  for (uint8_t i = 0; i < 3; i++) { v[i] = umaLeituraCm(); delay(15); }
  if (v[0] > v[1]) { float t = v[0]; v[0] = v[1]; v[1] = t; }
  if (v[1] > v[2]) { float t = v[1]; v[1] = v[2]; v[2] = t; }
  if (v[0] > v[1]) { float t = v[0]; v[0] = v[1]; v[1] = t; }
  return v[1];
}

// ------------------- Calibracao via Serial ---------------------------
void mostrarReferencias() {
  Serial.println(F(">> Referencias atuais [rn gn bn]:"));
  for (int i = 0; i < 4; i++) {
    Serial.print(F("   "));
    Serial.print(nomeCor[i]);
    Serial.print('\t');
    Serial.print(refCor[i][0], 3); Serial.print(' ');
    Serial.print(refCor[i][1], 3); Serial.print(' ');
    Serial.println(refCor[i][2], 3);
  }
}

void tratarSerial(float rn, float gn, float bn) {
  if (!Serial.available()) return;
  char c = Serial.read();
  int idx = -1;
  if      (c == 'r') idx = 0;
  else if (c == 'g') idx = 1;
  else if (c == 'b') idx = 2;
  else if (c == 'y') idx = 3;
  else if (c == 'p') { mostrarReferencias(); return; }
  else return;                       // ignora outras teclas (ex: Enter)

  refCor[idx][0] = rn;
  refCor[idx][1] = gn;
  refCor[idx][2] = bn;
  Serial.print(F(">> Referencia salva: "));
  Serial.println(nomeCor[idx]);
}

// =====================================================================
void loop() {
  float rn, gn, bn;
  uint16_t clear;
  lerCor(rn, gn, bn, clear);

  tratarSerial(rn, gn, bn);          // captura calibracao, se houver tecla

  float distRef;
  const char* cor = classificarCor(rn, gn, bn, clear, distRef);

  float dist = lerDistanciaCm();

  // ----- Saida no Monitor Serial -----
  Serial.print(F("COR: "));
  Serial.print(cor);
  Serial.print(F("   [rn:")); Serial.print(rn, 2);
  Serial.print(F(" gn:"));    Serial.print(gn, 2);
  Serial.print(F(" bn:"));    Serial.print(bn, 2);
  Serial.print(F(" clear:")); Serial.print(clear);
  Serial.print(F("]   |   DISTANCIA: "));
  if (dist < 0) Serial.print(F("fora de alcance"));
  else { Serial.print(dist, 1); Serial.print(F(" cm")); }
  Serial.println();

  delay(300);
}
