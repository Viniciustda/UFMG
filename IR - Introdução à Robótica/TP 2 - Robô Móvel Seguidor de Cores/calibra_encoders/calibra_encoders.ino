/*
  =====================================================================
  Calibracao MOTORIZADA dos encoders - TP2 Robo Artista
  ---------------------------------------------------------------------
  O robo se move sozinho um numero fixo de pulsos e para. Voce mede o
  resultado (distancia ou angulo) parado, e a gente calcula:
      COUNTS_PER_CM  = pulsos_da_reta  / distancia_medida_cm
      COUNTS_PER_DEG = pulsos_do_giro  / angulo_total_em_graus
  ---------------------------------------------------------------------
  Ligacoes: Adafruit Motor Shield V2 (motores M1/M2),
            encoders LM393 em 18 e 19, LCD Keypad Shield.
  ---------------------------------------------------------------------
  USO (botoes do LCD Keypad Shield):
    SELECT  -> TESTE DE RETA: anda ~PULSOS_FRENTE pulsos e para.
               Depois meca com uma trena a distancia percorrida.
    CIMA    -> TESTE DE GIRO: gira ~PULSOS_GIRO pulsos e para (devagar).
               Depois meca o angulo total: conte as voltas completas
               que ele deu e some o pedaco que sobrou.
    ESQUERDA-> aborta e volta ao menu.

  O numero de pulsos REAL (ja incluindo o pequeno deslize ao frear)
  aparece no LCD e no Serial Monitor (115200 baud). Use ESSE numero,
  nao o alvo, para fazer as contas.
  =====================================================================
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <LiquidCrystal.h>
#include <stdio.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const uint8_t PIN_ENC_ESQ = 18;
const uint8_t PIN_ENC_DIR = 19;
const uint8_t PIN_BOTOES  = A0;

// --- alvos dos testes (pode mudar) ---
const long PULSOS_FRENTE = 190;   // pulsos no teste de reta
const long PULSOS_GIRO   = 120;   // pulsos no teste de giro

// --- velocidades (0..255) ---
const int   VEL_FRENTE = 130;
const int   VEL_GIRO   = 90;      // devagar, para dar pra contar as voltas
const float KP_RETO    = 3.0;     // correcao para andar reto

// --- motores (Motor Shield V2) ---
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motEsq;
Adafruit_DCMotor *motDir;
const uint8_t PORTA_MOT_ESQ = 1;          // M1
const uint8_t PORTA_MOT_DIR = 2;          // M2
const bool    INV_ESQ = false;            // troque se a roda girar ao contrario
const bool    INV_DIR = false;

// --- encoders com filtro de ruido ---
volatile long encEsq = 0;
volatile long encDir = 0;
volatile unsigned long tUltEsq = 0;
volatile unsigned long tUltDir = 0;
const unsigned long DEBOUNCE_US = 1500;
void isrEsq() {
  unsigned long a = micros();
  if (a - tUltEsq < DEBOUNCE_US) return;
  tUltEsq = a; encEsq++;
}
void isrDir() {
  unsigned long a = micros();
  if (a - tUltDir < DEBOUNCE_US) return;
  tUltDir = a; encDir++;
}
void zera() { noInterrupts(); encEsq = 0; encDir = 0; interrupts(); }
long lerE() { noInterrupts(); long v = encEsq; interrupts(); return v; }
long lerD() { noInterrupts(); long v = encDir; interrupts(); return v; }
long encMedia() { return (lerE() + lerD()) / 2; }

// --- estado ---
enum { ET_MENU, ET_RETA, ET_GIRO, ET_FIM_RETA, ET_FIM_GIRO };
uint8_t etapa = ET_MENU;
long pulsosFeitos = 0;

// --- botoes do LCD Keypad Shield ---
enum { BT_NENHUM, BT_DIR, BT_CIMA, BT_BAIXO, BT_ESQ, BT_SELECT };
int leBotao() {
  int v = analogRead(PIN_BOTOES);
  if (v <  60) return BT_DIR;
  if (v < 250) return BT_CIMA;
  if (v < 450) return BT_BAIXO;
  if (v < 650) return BT_ESQ;
  if (v < 850) return BT_SELECT;
  return BT_NENHUM;
}
int eventoBotao() {
  static int ant = BT_NENHUM;
  int a = leBotao();
  int ev = (a != BT_NENHUM && ant == BT_NENHUM) ? a : BT_NENHUM;
  ant = a;
  return ev;
}

// --- motores ---
void setMotor(uint8_t lado, int spd) {
  bool inv = (lado == 0) ? INV_ESQ : INV_DIR;
  Adafruit_DCMotor *m = (lado == 0) ? motEsq : motDir;
  if (inv) spd = -spd;
  if (spd >  255) spd =  255;
  if (spd < -255) spd = -255;
  if (spd > 0)      { m->setSpeed(spd);  m->run(FORWARD); }
  else if (spd < 0) { m->setSpeed(-spd); m->run(BACKWARD); }
  else              { m->setSpeed(0);    m->run(RELEASE); }
}
void para() { setMotor(0, 0); setMotor(1, 0); }

// para os motores e espera o robo terminar de deslizar;
// devolve a contagem final de pulsos (ja com o deslize)
long esperaParar() {
  para();
  long ult = encMedia();
  unsigned long t0 = millis();
  while (millis() - t0 < 700) {        // estavel por 700 ms = parou
    delay(50);
    long ag = encMedia();
    if (ag != ult) { ult = ag; t0 = millis(); }
  }
  return encMedia();
}

// --- LCD ---
void lcdLinha(uint8_t lin, const char* txt) {
  lcd.setCursor(0, lin);
  char b[17];
  uint8_t i = 0;
  while (i < 16 && txt[i] != '\0') { b[i] = txt[i]; i++; }
  while (i < 16) { b[i] = ' '; i++; }
  b[16] = '\0';
  lcd.print(b);
}

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);

  pinMode(PIN_ENC_ESQ, INPUT_PULLUP);
  pinMode(PIN_ENC_DIR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_ESQ), isrEsq, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_DIR), isrDir, RISING);

  Wire.begin();
  AFMS.begin();
  motEsq = AFMS.getMotor(PORTA_MOT_ESQ);
  motDir = AFMS.getMotor(PORTA_MOT_DIR);
  para();

  Serial.println(F("=== Calibracao motorizada dos encoders ==="));
  lcdLinha(0, "Calib. motor");
  lcdLinha(1, "SEL=reta CIMA=giro");
  delay(1500);
}

void loop() {
  int botao = eventoBotao();

  switch (etapa) {

    case ET_MENU:
      para();
      lcdLinha(0, "Calibracao motor");
      lcdLinha(1, "SEL=reta CIMA=giro");
      if (botao == BT_SELECT)    { zera(); etapa = ET_RETA; }
      else if (botao == BT_CIMA) { zera(); etapa = ET_GIRO; }
      break;

    case ET_RETA: {
      long m = encMedia();
      if (m >= PULSOS_FRENTE) {
        pulsosFeitos = esperaParar();
        Serial.print(F("RETA terminou: "));
        Serial.print(pulsosFeitos);
        Serial.println(F(" pulsos. Meca a distancia percorrida (cm)."));
        etapa = ET_FIM_RETA;
      } else {
        int corr = (int)(KP_RETO * (lerE() - lerD()));   // mantem reto
        setMotor(0, VEL_FRENTE - corr);
        setMotor(1, VEL_FRENTE + corr);
        char l1[20];
        snprintf(l1, sizeof(l1), "pulsos %ld/%ld", m, PULSOS_FRENTE);
        lcdLinha(0, "Andando reto...");
        lcdLinha(1, l1);
      }
      if (botao == BT_ESQ) { para(); etapa = ET_MENU; }
      break;
    }

    case ET_GIRO: {
      long m = encMedia();
      if (m >= PULSOS_GIRO) {
        pulsosFeitos = esperaParar();
        Serial.print(F("GIRO terminou: "));
        Serial.print(pulsosFeitos);
        Serial.println(F(" pulsos. Meca o angulo total girado (graus)."));
        etapa = ET_FIM_GIRO;
      } else {
        setMotor(0,  VEL_GIRO);          // gira no proprio eixo
        setMotor(1, -VEL_GIRO);
        char l1[20];
        snprintf(l1, sizeof(l1), "pulsos %ld/%ld", m, PULSOS_GIRO);
        lcdLinha(0, "Girando devagar.");
        lcdLinha(1, l1);
      }
      if (botao == BT_ESQ) { para(); etapa = ET_MENU; }
      break;
    }

    case ET_FIM_RETA: {
      para();
      char l0[20];
      snprintf(l0, sizeof(l0), "RETA: %ld pulsos", pulsosFeitos);
      lcdLinha(0, l0);
      lcdLinha(1, "Meca a distancia");
      if (botao != BT_NENHUM) etapa = ET_MENU;
      break;
    }

    case ET_FIM_GIRO: {
      para();
      char l0[20];
      snprintf(l0, sizeof(l0), "GIRO: %ld pulsos", pulsosFeitos);
      lcdLinha(0, l0);
      lcdLinha(1, "Meca os graus");
      if (botao != BT_NENHUM) etapa = ET_MENU;
      break;
    }
  }

  delay(15);
}
