#pragma once
/*
  sensores.h - Ultrassom HC-SR04, sensor de cor TCS34725 e LDRs
*/

#include <Arduino.h>
#include "Adafruit_TCS34725.h"

extern Adafruit_TCS34725 tcs;

extern int leituraDistAtual;   // ultima distancia lida (cm), -1 = nada
extern int corVistaAtual;      // ultima cor vista (-1 = nenhuma)
extern int corDetectada;       // cor do bloco que disparou a reacao

extern float refCor[4][3];     // calibracao [cor][rn,gn,bn]

bool iniciaSensores();         // tcs.begin(), retorna false se nao achou
float leDistanciaCm();
void leCromaticidade(float &rn, float &gn, float &bn, uint16_t &clear, uint8_t n);
int  classificaCromaticidade(float rn, float gn, float bn, uint16_t clear);
int  classificaCor(uint8_t n);
uint8_t monitoraSensores();    // 0 nada, 1 parede, 2 bloco (corDetectada)

// LDRs de linha (saida digital): true se o sensor esta sobre o preto
bool linhaEsqPreto();
bool linhaDirPreto();

// LDRs de alinhamento (entrada analogica): valor bruto lido (0..1023)
int leLdrTesteEsq();
int leLdrTesteDir();

// LDR inferior (saida digital): true quando a luz esta acesa
bool luzAcesa();
