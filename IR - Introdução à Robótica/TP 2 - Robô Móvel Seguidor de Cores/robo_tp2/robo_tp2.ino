/*
  TP2
  ---------------------------------------------------------------------
  HARDWARE
    - Arduino Mega 2560
    - Adafruit Motor Shield V2 (I2C 0x60): motor esq = M1, motor dir = M2
    - Sensor de cor TCS34725 (I2C 0x29) : SDA 20, SCL 21
    - Ultrassom HC-SR04                 : Trig 41, Echo 43
    - Encoders LM393 (disco 20 furos)   : Esq 18, Dir 19  (interrupcao)
    - LCD Keypad Shield                 : LCD 8,9,4,5,6,7 ; botoes em A0
  ---------------------------------------------------------------------
  MENU
    1 Linha reta   - frente 30, gira 180, volta 30, gira 180
    2 Triangulo    - 30 / gira135 / hipotenusa / gira135 / 30 / gira90
    3 Quadrado     - 4x (frente 30 + gira 90 a direita)
    4 Identificar  - parado, mostra no LCD a cor que esta na frente
    5 Reta+blocos  - anda reto e reage a cor dos blocos
    6 Calibrar cor - calibra as 4 cores + a parede branca
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "Adafruit_TCS34725.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <string.h>
#include <stdio.h>

// ======================= PINOS =======================================
const uint8_t PIN_TRIG    = 41;
const uint8_t PIN_ECHO    = 43;
const uint8_t PIN_ENC_ESQ = 18;
const uint8_t PIN_ENC_DIR = 19;
const uint8_t PIN_BOTOES  = A0;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// ======================= MOTORES =====================================
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motEsq;
Adafruit_DCMotor *motDir;
const uint8_t PORTA_MOT_ESQ = 1;       // M1
const uint8_t PORTA_MOT_DIR = 2;       // M2

// ======================= SENSOR DE COR ===============================
// Integracao de 50 ms: Menos ruido
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// ================== VELOCIDADES (0, 255) =========================
const int VEL_FRENTE = 75;    // andando reto
const int VEL_LENTA  = 60;    // aproximacao lenta perto de objeto
const int VEL_GIRO   = 70;    // velocidade dos giros
const int VEL_RE     = 65;    // velocidade da re
const int VEL_MIN    = 55;    // velocidade minima

// Geometria / encoders -> Calculado empiricamente
float COUNTS_PER_CM  = 1.99;    // calibrado: 209 pulsos / 105 cm
float COUNTS_PER_DEG = 0.2016;  // calibrado: 127 pulsos / 630 graus
const float KP_RETO  = 3.0;   // Controlador proporcional nas retas

// Inercia: Desliga os motores X pulsos antes do final
const long FOLGA_RETA = 6;
const long FOLGA_GIRO = 0;   // Usado pra teste

// Distancias dos trajetos (cm)
const float DIST_LADO       = 30.0;
const float DIST_HIPOTENUSA = 42.43;
const float DIST_RE         = 7.0;   // Re antes de girar quando identifica bloco

// Deteccao
const float DIST_DETECTA = 8.0;       // comeca a observar e desacelerar
const float DIST_COR_MAX = 4.0;       // Dist para agir com base na cor
const float DIST_PAREDE  = 2.0;       // Para nesta distancia de paredes
const unsigned long TIMEOUT_BLOCO = 10000UL;  // Para depois de 10s sem bloco

// ================== CLASSIFICACAO DE COR =============================
enum { COR_VERMELHO, COR_VERDE, COR_AZUL, COR_AMARELO, COR_BRANCO }; // Branco é usado para identificar a parede
const char* NOME_COR[5] = { "VERMELHO", "VERDE", "AZUL", "AMARELO", "BRANCO" };

// Calibracao padrao: [cor][rn,gn,bn]
float refCor[5][3] = {
  { 0.55, 0.27, 0.18 },    // VERMELHO
  { 0.27, 0.50, 0.23 },    // VERDE
  { 0.22, 0.33, 0.45 },    // AZUL
  { 0.45, 0.42, 0.13 },    // AMARELO
  { 0.333, 0.333, 0.334 }  // BRANCO / PAREDE
};
const uint16_t CLEAR_MIN       = 50;     // abaixo: escuro / sem objeto
const float    DIST_MAX        = 0.30;   // acima: cor nao reconhecida
const float    DIST_MAX_BRANCO = 0.020;  // Diferenca maxima entre branco detectado e o calibrado para considerar parede
const uint8_t  EEPROM_MAGIC = 0xC4;

// ======================= ESTADO GLOBAL ===============================
enum { EST_MENU, EST_PASSOS, EST_LINHA, EST_COR, EST_CALIB, EST_PARADO };
uint8_t estado = EST_MENU;

enum { BT_NENHUM, BT_DIR, BT_CIMA, BT_BAIXO, BT_ESQ, BT_SELECT };

const char* ITENS_MENU[6] = {
  "1 Linha reta", "2 Triangulo", "3 Quadrado",
  "4 Identif cor", "5 Reta+blocos", "6 Calibrar cor"
};
uint8_t menuIdx   = 0;
uint8_t trajetoId = 0;
uint8_t calibIdx  = 0;

int  leituraDistAtual = -1;           // ultima distancia lida (cm)
int  corVistaAtual    = -1;           // ultima cor vista (-1 = nenhuma)
int  corDetectada     = -1;           // cor do bloco que disparou a reacao
unsigned long tempoUltimoBloco = 0;
char msgParado[17] = "";

// ---- encoders ----
volatile long encEsq = 0;
volatile long encDir = 0;
volatile unsigned long tUltEsq = 0; // tempo do ultimo pulso no esquerdo
volatile unsigned long tUltDir = 0; // tempo do ultimo pulso no direito
const unsigned long DEBOUNCE_US = 1500; // so considera outro pulso se passar 1,5 ms

// ---- Variaveis de movimento ----
enum { MK_FRENTE, MK_RE, MK_GIRA_DIR, MK_GIRA_ESQ };
uint8_t movKind  = MK_FRENTE;
long    movAlvo  = 0;
bool    movAtivo = false;
unsigned long tParou = 0;             // instante em que o ultimo movimento parou
const unsigned long SETTLE_MS = 400;  // pausa entre passos

// ---- lista de passos ----
enum { ST_FRENTE, ST_RE, ST_GIRA };
enum { PROP_TRAJETO, PROP_REACAO }; // se robo esta em trajeto ou reagindo a bloco
#define MAX_PASSOS 12
uint8_t passoTipo[MAX_PASSOS];
float   passoValor[MAX_PASSOS];
int8_t  passoDir[MAX_PASSOS];         // +1 direita, -1 esquerda
uint8_t numPassos   = 0;
uint8_t passoAtual  = 0;
uint8_t propositoPassos = PROP_TRAJETO;

// ========================= SETUP =====================================
void setup() {
  Serial.begin(115200);
  Serial.println(F("TP2 - Versao Final 2.0"));

  lcd.begin(16, 2);
  lcd.print("Gambiarra Tech");

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  pinMode(PIN_ENC_ESQ, INPUT_PULLUP);
  pinMode(PIN_ENC_DIR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_ESQ), isrEsq, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_DIR), isrDir, RISING);

  Wire.begin();
  AFMS.begin();  // Motorshield v2
  motEsq = AFMS.getMotor(PORTA_MOT_ESQ);
  motDir = AFMS.getMotor(PORTA_MOT_DIR);
  paraMotores();

  if (!tcs.begin()) {
    lcd.setCursor(0, 1);
    lcd.print("Erro TCS34725");
    Serial.println(F("ERRO: TCS34725 nao encontrado."));
  }

  carregaEEPROM();                    // carrega calibracao de cor da eeprom

  Serial.println(F("Calibracao de cor: use a opcao 6 do menu"));
  Serial.println(F(" ou envie 'c' pelo PC para fazer pelo pc"));

  delay(900);
  estado = EST_MENU;
}

// ========================= LOOP ======================================
void loop() {
  int botao = eventoBotao();
  calibraPelaSerial();         

  // Botao ESQUERDA: volta no menu
  if (botao == BT_ESQ &&
      (estado == EST_PASSOS || estado == EST_LINHA || estado == EST_COR)) {
    paraMotores();
    movAtivo = false;
    estado = EST_MENU;
    botao = BT_NENHUM;
  }

  switch (estado) {
    case EST_MENU:   atualizaMenu(botao);   break;
    case EST_PASSOS: atualizaPassos();      break;
    case EST_LINHA:  atualizaLinha();       break;
    case EST_COR:    atualizaCor();         break;
    case EST_CALIB:  atualizaCalib(botao);  break;
    case EST_PARADO:
      paraMotores();
      if (botao == BT_SELECT) estado = EST_MENU;
      break;
  }

  atualizaLCD();
}

// ===================== ENCODERS ======================================
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

// ===================== MOTORES =======================================
// 0 = esquerdo, 1 = direito ; velocidade: (-255,255)
// Coloca velocidade no motor
void setMotor(uint8_t lado, int spd) {
  Adafruit_DCMotor *m = (lado == 0) ? motEsq : motDir;
  if (spd >  255) spd =  255;
  if (spd < -255) spd = -255;
  if (spd > 0)      { m->setSpeed(spd);  m->run(FORWARD); }
  else if (spd < 0) { m->setSpeed(-spd); m->run(BACKWARD); }
  else              { m->setSpeed(0);    m->run(RELEASE); }
}
void paraMotores() {
  setMotor(0, 0);
  setMotor(1, 0);
}

// ===================== SISTEMA DE MOVIMENTO ==========================
void iniciaMovimento(uint8_t kind, long alvo) {
  zeraEncoders();
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
    int corr = (int)(KP_RETO * (e - d));  // Controlador proporcional
    int vE = VEL_FRENTE - corr;
    int vD = VEL_FRENTE + corr;
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
    setMotor(0,  VEL_GIRO);
    setMotor(1, -VEL_GIRO);
  } else {
    setMotor(0, -VEL_GIRO);
    setMotor(1,  VEL_GIRO);
  }
  return true;
}

// ===================== LISTA DE PASSOS ===============================
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
  else
    iniciaMovimento(passoDir[i] > 0 ? MK_GIRA_DIR : MK_GIRA_ESQ, (long)(passoValor[i] * COUNTS_PER_DEG) - FOLGA_GIRO);
}

void carregaTrajeto(uint8_t id) {
  numPassos = 0;
  trajetoId = id;
  if (id == 1) {                       // Linha reta
    addPasso(ST_FRENTE, DIST_LADO, 0);
    addPasso(ST_GIRA,   180,      +1);
    addPasso(ST_FRENTE, DIST_LADO, 0);
    addPasso(ST_GIRA,   180,      +1);
  } else if (id == 2) {                // Triangulo
    addPasso(ST_FRENTE, DIST_LADO,       0);
    addPasso(ST_GIRA,   90,             +1);
    addPasso(ST_FRENTE, DIST_LADO,       0);
    addPasso(ST_GIRA,   135,            +1);
    addPasso(ST_FRENTE, DIST_HIPOTENUSA, 0);
    addPasso(ST_GIRA,   135,            +1);
  } else {                             // Quadrado 
    for (uint8_t k = 0; k < 4; k++) {
      addPasso(ST_FRENTE, DIST_LADO, 0);
      addPasso(ST_GIRA,   90,       +1);
    }
  }
  propositoPassos = PROP_TRAJETO;
  passoAtual = 0;
  iniciaPasso(0);
}

// Reacao a bloco: recua antes de agir
void carregaReacao(int cor) {
  numPassos = 0;
  addPasso(ST_RE, DIST_RE, 0);
  if (cor == COR_VERMELHO)      addPasso(ST_GIRA, 360, +1);  // gira 360 e para
  else if (cor == COR_VERDE)    addPasso(ST_GIRA,  90, -1);  // esquerda 90
  else if (cor == COR_AZUL)     addPasso(ST_GIRA,  90, +1);  // direita 90
  else                          addPasso(ST_GIRA, 180, +1);  // amarelo: 180
  propositoPassos = PROP_REACAO;
  passoAtual = 0;
  iniciaPasso(0);
}

// ===================== SENSORES ======================================
float leDistanciaCm() {
  digitalWrite(PIN_TRIG, LOW);  delayMicroseconds(3);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  unsigned long dur = pulseIn(PIN_ECHO, HIGH, 25000UL);  // timeout ~4 m
  if (dur == 0) return -1.0;
  return dur * 0.0343 / 2.0;
}

// Filtro de media movel - Retorna cromaticidade normalizada pela intensidade
// Estamos usando n=5 na calibração e n=2 quando se move
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

// Classifica cor: 0,1,23 = bloco, 4 = branco/parede,
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
  for (int i = 0; i < 4; i++) {
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
    if (c >= 0 && c <= 3) { corDetectada = c; return 2; } // Retorna bloco
  } else {
    corVistaAtual = -1;                      
  }
  if (d <= DIST_PAREDE) return 1;               // parede
  return 0;                                     // aproximando
}

// ===================== MODOS =========================================
// Executa a lista de passos
void atualizaPassos() {
  bool emMovimento = atualizaMovimento();

  if (emMovimento) {
    if (passoTipo[passoAtual] == ST_FRENTE) {
      uint8_t ev = monitoraSensores();
      if (ev == 2) {                            // achou um bloco
        paraMotores();
        Serial.print(F("Bloco detectado: "));
        Serial.println(NOME_COR[corDetectada]);
        carregaReacao(corDetectada);            // abandona o trajeto
      } else if (ev == 1) {                     // parede
        paraMotores();
        movAtivo = false;
        strcpy(msgParado, "Parede! Parado");
        estado = EST_PARADO;
      }
    }
    return;
  }

  // movimento terminou: espera um tempo
  if (millis() - tParou < SETTLE_MS) { paraMotores(); return; }

  // Proximo passo
  passoAtual++;
  if (passoAtual < numPassos) {
    iniciaPasso(passoAtual);
    return;
  }

  // a sequencia inteira terminou
  if (propositoPassos == PROP_TRAJETO) {
    strcpy(msgParado, "Trajeto OK");
    estado = EST_PARADO;
  } else {                                      // terminou uma reacao
    if (corDetectada == COR_VERMELHO) {
      strcpy(msgParado, "Bloco vermelho");      // vermelho: para apos o giro
      estado = EST_PARADO;
    } else {
      entraLinha();                             // segue procurando blocos
    }
  }
}

// Modo 5 (e continuacao de 1/2/3 apos achar bloco): anda reto e reage
void atualizaLinha() {
  uint8_t ev = monitoraSensores();

  if (ev == 2) {                                // bloco
    paraMotores();
    Serial.print(F("Bloco detectado: "));
    Serial.println(NOME_COR[corDetectada]);
    carregaReacao(corDetectada);
    estado = EST_PASSOS;
    return;
  }
  if (ev == 1) {                                // parede
    paraMotores();
    strcpy(msgParado, "Parede! Parado");
    estado = EST_PARADO;
    return;
  }
  if (millis() - tempoUltimoBloco > TIMEOUT_BLOCO) {   // 10 s sem bloco
    paraMotores();
    strcpy(msgParado, "10s sem bloco");
    estado = EST_PARADO;
    return;
  }

  // Segue reto
  int vel = VEL_FRENTE;
  if (leituraDistAtual >= 0 && leituraDistAtual <= (int)DIST_DETECTA)
    vel = VEL_LENTA;   // Mais devagar se ta aproximando
  long e = lerEncEsq();
  long d = lerEncDir();
  int corr = (int)(KP_RETO * (e - d));
  int vE = vel - corr;
  int vD = vel + corr;
  if (vE < VEL_MIN) vE = VEL_MIN;
  if (vD < VEL_MIN) vD = VEL_MIN;
  if (vE > 255)     vE = 255;
  if (vD > 255)     vD = 255;
  setMotor(0, vE);
  setMotor(1, vD);
}

// Modo 4: parado, apenas identifica a cor na frente
void atualizaCor() {
  paraMotores();
  static unsigned long ultimo = 0;
  if (millis() - ultimo > 250) {
    ultimo = millis();
    float d = leDistanciaCm();
    leituraDistAtual = (d < 0) ? -1 : (int)d;
    // so identifica se o objeto estiver perto o bastante (<= 4 cm)
    if (d > 0 && d <= DIST_COR_MAX) corVistaAtual = classificaCor(4);
    else                            corVistaAtual = -2;   // -2 = aproxime mais
  }
}

// Modo 6: calibracao das cores (4 blocos + parede branca)
void atualizaCalib(int botao) {
  paraMotores();

  // a captura pode vir do botao SELECT no LCD OU pelo pc
  bool capturar = (botao == BT_SELECT);
  if (Serial.available()) {
    char c = Serial.read();
    if (c != '\n' && c != '\r' && c != ' ') capturar = true;
  }
  if (botao == BT_ESQ) { estado = EST_MENU; return; }   // cancela

  if (!capturar) return;

  float rn, gn, bn;
  uint16_t cl;
  leCromaticidade(rn, gn, bn, cl, 5);          // media de 5
  refCor[calibIdx][0] = rn;
  refCor[calibIdx][1] = gn;
  refCor[calibIdx][2] = bn;
  Serial.print(F(">> "));
  Serial.print(NOME_COR[calibIdx]);
  Serial.print(F(" calibrado: "));
  Serial.print(rn, 3); Serial.print(' ');
  Serial.print(gn, 3); Serial.print(' ');
  Serial.print(bn, 3);
  Serial.print(F("  (clear=")); Serial.print(cl); Serial.println(F(")"));

  calibIdx++;
  if (calibIdx >= 5) {                          // 4 cores + branco prontos
    salvaEEPROM();
    strcpy(msgParado, "Calibracao OK");
    estado = EST_PARADO;
  } else {
    Serial.print(F("Agora mostre: "));
    Serial.print(NOME_COR[calibIdx]);
    if (calibIdx == COR_BRANCO) Serial.println(F(" (aponte para a parede)"));
    else                        Serial.println();
  }
}

// Menu principal
void atualizaMenu(int botao) {
  paraMotores();
  if (botao == BT_CIMA  && menuIdx > 0) menuIdx--;
  if (botao == BT_BAIXO && menuIdx < 5) menuIdx++;
  if (botao == BT_SELECT) {
    if (menuIdx <= 2)      { carregaTrajeto(menuIdx + 1); estado = EST_PASSOS; }
    else if (menuIdx == 3) { corVistaAtual = -1; estado = EST_COR; }
    else if (menuIdx == 4) { entraLinha(); }
    else                   { entraCalib(); }
  }
}

void entraLinha() {
  zeraEncoders();
  movAtivo = false;
  tempoUltimoBloco = millis();
  corVistaAtual = -1;
  estado = EST_LINHA;
}

void entraCalib() {
  calibIdx = 0;
  estado = EST_CALIB;
  Serial.println(F("--- Calibracao de cor ---"));
  Serial.println(F("Mostre cada cor a ~4 cm e aperte SELECT (LCD) ou qualquer tecla (PC)."));
  Serial.println(F("Sao 5 capturas: VERMELHO, VERDE, AZUL, AMARELO e a PAREDE branca."));
  Serial.print(F("Agora mostre: "));
  Serial.println(NOME_COR[0]);
}

// ===================== BOTOES (LCD) ====================
int leBotaoBruto() {
  int v = analogRead(PIN_BOTOES);
  if (v <  60) return BT_DIR;
  if (v < 250) return BT_CIMA;
  if (v < 450) return BT_BAIXO;
  if (v < 650) return BT_ESQ;
  if (v < 850) return BT_SELECT;
  return BT_NENHUM;
}

// So retorna o botao no instante em que ele eh pressionado 
int eventoBotao() {
  static int anterior = BT_NENHUM;
  int atual = leBotaoBruto();
  int ev = BT_NENHUM;
  if (atual != BT_NENHUM && anterior == BT_NENHUM) {
    ev = atual;
  }
  anterior = atual;
  return ev;
}

// ===================== EEPROM ====================
void carregaEEPROM() {
  if (EEPROM.read(0) == EEPROM_MAGIC) {
    EEPROM.get(1, refCor);
    Serial.println(F("Calibracao de cor carregada da EEPROM."));
  } else {
    Serial.println(F("Sem calibracao salva - usando valores padrao."));
  }
}
void salvaEEPROM() {
  EEPROM.put(1, refCor);
  EEPROM.write(0, EEPROM_MAGIC);
  Serial.println(F("Calibracao salva na EEPROM."));
}

// ============== ATALHO DE CALIBRACAO PELO PC (Serial) ================
// Com o robo no menu, envie 'c' pelo Monitor Serial (115200 baud)
void calibraPelaSerial() {
  if (estado != EST_MENU) return;
  if (!Serial.available()) return;
  char c = Serial.read();
  if (c == 'c' || c == 'C') entraCalib();
}

// ===================== LCD ===========================================
// escreve um texto ocupando exatamente 16 colunas (sem piscar a tela)
void lcdLinha(uint8_t lin, const char* txt) {
  lcd.setCursor(0, lin);
  char buf[17];
  uint8_t i = 0;
  while (i < 16 && txt[i] != '\0') { buf[i] = txt[i]; i++; }
  while (i < 16) { buf[i] = ' '; i++; }
  buf[16] = '\0';
  lcd.print(buf);
}

void atualizaLCD() {
  static unsigned long ultimo = 0;
  if (millis() - ultimo < 250) return;
  ultimo = millis();

  char l0[24], l1[24], db[8];
  if (leituraDistAtual < 0) strcpy(db, "--");
  else snprintf(db, sizeof(db), "%d", leituraDistAtual);

  switch (estado) {
    case EST_MENU:
      strcpy(l0, "TP2  Selecione:");
      snprintf(l1, sizeof(l1), ">%s", ITENS_MENU[menuIdx]);
      break;
    case EST_PASSOS:
      if (propositoPassos == PROP_REACAO)
        snprintf(l0, sizeof(l0), "Reage: %s", NOME_COR[corDetectada]);
      else
        snprintf(l0, sizeof(l0), "Trajeto %d", trajetoId);
      snprintf(l1, sizeof(l1), "Passo %d/%d d:%s",
               passoAtual + 1, numPassos, db);
      break;
    case EST_LINHA:
      strcpy(l0, "Reta + blocos");
      if (corVistaAtual >= 0)
        snprintf(l1, sizeof(l1), "d:%s cor:%s", db, NOME_COR[corVistaAtual]);
      else
        snprintf(l1, sizeof(l1), "d:%s procurando", db);
      break;
    case EST_COR:
      strcpy(l0, "Identificar cor");
      if (corVistaAtual == -2)
        snprintf(l1, sizeof(l1), "d:%s  aproxime", db);
      else if (corVistaAtual >= 0)
        snprintf(l1, sizeof(l1), "Cor: %s", NOME_COR[corVistaAtual]);
      else
        snprintf(l1, sizeof(l1), "d:%s  Cor: ---", db);
      break;
    case EST_CALIB:
      snprintf(l0, sizeof(l0), "Mostre %s", NOME_COR[calibIdx]);
      if (calibIdx == COR_BRANCO) strcpy(l1, "=parede  SEL/PC");
      else                        strcpy(l1, "SEL ou tecla PC");
      break;
    default: // EST_PARADO
      strcpy(l0, msgParado);
      strcpy(l1, "SELECT p/ menu");
      break;
  }
  lcdLinha(0, l0);
  lcdLinha(1, l1);
}