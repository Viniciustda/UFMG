#pragma once
/*
  modos.h - Maquina de estados: menu e modos de operacao.
*/

#include <Arduino.h>

extern uint8_t estado;
extern uint8_t menuIdx;
extern uint8_t trajTipo;   // TRAJ_RETA ou TRAJ_QUADRADO escolhido
extern uint8_t tamCm;      // tamanho escolhido (cm), ajustavel de 10 em 10
extern uint8_t calibIdx;
extern uint8_t alFase;     // fase do alinhamento
extern char msgParado[17];

// Acao a executar quando o alinhamento terminar
enum { ACAO_NENHUMA, ACAO_PASSOS, ACAO_LINHA };
extern uint8_t acaoAposAlinhar;
extern bool    alinhaAntes;   // true = alinhar apos escolher o tamanho (reta/quadrado)
extern bool          reagindoBloco;   // true durante a pausa de reacao a bloco
extern unsigned long tBloco;          // instante em que o bloco foi detectado
extern char          msgBloco[17];    // "Bloco: <cor>" exibido na pausa

void atualizaPassos();
void atualizaTamanho(int botao);
void atualizaSegueLinha();
void atualizaTesteLdr();
void iniciaTestaAlinhamento();
void atualizaTestaAlinhamento();
void atualizaCor();
void atualizaCalib(int botao);
void atualizaMenu(int botao);
void entraCalib();
void calibraPelaSerial();
