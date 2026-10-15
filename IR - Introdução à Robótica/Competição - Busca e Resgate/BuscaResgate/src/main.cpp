/*
  TP3
  
  HARDWARE
    - Arduino Mega 2560
    - Adafruit Motor Shield V2 (I2C 0x60): motor esq = M1, motor dir = M2
    - Sensor de cor TCS34725 (I2C 0x29) : SDA 20, SCL 21
    - Ultrassom HC-SR04                 : Trig 41, Echo 43
    - Encoders LM393 (disco 20 furos)   : Esq 18, Dir 19  (interrupcao)
    - Sensores de linha (LDR digital)   : Esq 34, Dir 35
    - LCD Keypad Shield                 : LCD 8,9,4,5,6,7 ; botoes em A0
  
  MENU
    1 Reta         - escolhe tamanho; frente N, gira 180, volta N, gira 180
    2 Quadrado     - escolhe tamanho; 4x (frente N + gira 90 a direita)
    3 Seguir linha - segue a linha preta com os 2 LDRs
    4 Identificar  - parado, mostra no LCD a cor que esta na frente
    5 Calibrar cor - calibra as 3 cores + a parede branca
    6 Teste LDR    - parado, mostra no LCD a leitura dos 2 LDRs (A10/A11)
    7 Alinhamento  - ao apagar a luz (LDR pino 51), gira 360 e alinha no
                     minimo de min(LDR esq, LDR dir)

*/

#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "encoders.h"
#include "motores.h"
#include "movimento.h"
#include "sensores.h"
#include "botoes.h"
#include "armazenamento.h"
#include "display.h"
#include "modos.h"


void setup() {
  Serial.begin(115200);
  Serial.println(F("TP3 - Versao Final"));

  iniciaLCD();

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  pinMode(PIN_LDR_ESQ, INPUT);
  pinMode(PIN_LDR_DIR, INPUT);
  pinMode(PIN_LDR_BAIXO, INPUT);

  pinMode(PIN_ENC_ESQ, INPUT_PULLUP);
  pinMode(PIN_ENC_DIR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_ESQ), isrEsq, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_DIR), isrDir, RISING);

  Wire.begin();
  iniciaMotores();

  if (!iniciaSensores()) {
    lcd.setCursor(0, 1);
    lcd.print("Erro TCS34725");
    Serial.println(F("ERRO: TCS34725 nao encontrado."));
  }

  carregaEEPROM();                    // carrega calibracao de cor da eeprom

  Serial.println(F("Calibracao de cor: use a opcao 6 do menu"));
  Serial.println(F(" ou envie 'c' pelo PC"));

  delay(900);
  estado = EST_MENU;
}


void loop() {
  int botao = eventoBotao();
  calibraPelaSerial();

  // Botao ESQUERDA: volta no menu
  if (botao == BT_ESQ &&
      (estado == EST_PASSOS || estado == EST_LINHA ||
       estado == EST_COR    || estado == EST_TESTE_LDR ||
       estado == EST_ALINHA)) {
    paraMotores();
    movAtivo = false;
    acaoAposAlinhar = ACAO_NENHUMA;   // cancela acao apos o alinhamento
    estado = EST_MENU;
    botao = BT_NENHUM;
  }

  switch (estado) {
    case EST_MENU:    atualizaMenu(botao);     break;
    case EST_TAMANHO: atualizaTamanho(botao);  break;
    case EST_PASSOS:  atualizaPassos();        break;
    case EST_LINHA:   atualizaSegueLinha();    break;
    case EST_TESTE_LDR: atualizaTesteLdr();    break;
    case EST_ALINHA:  atualizaTestaAlinhamento(); break;
    case EST_COR:     atualizaCor();           break;
    case EST_CALIB:   atualizaCalib(botao);    break;
    case EST_PARADO:
      paraMotores();
      if (botao == BT_SELECT) estado = EST_MENU;
      break;
  }

  atualizaLCD();
}
