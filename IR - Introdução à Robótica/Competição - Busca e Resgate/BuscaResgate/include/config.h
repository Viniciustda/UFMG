#pragma once
/*
  config.h - Constantes utilizadas
*/

#include <Arduino.h>

// ======================= PINOS =======================================
const uint8_t PIN_TRIG    = 41;
const uint8_t PIN_ECHO    = 43;
const uint8_t PIN_ENC_ESQ = 18;
const uint8_t PIN_ENC_DIR = 19;
const uint8_t PIN_BOTOES  = A0;
const uint8_t PIN_LDR_ESQ = 35;       // sensor de linha esquerdo (saida digital)
const uint8_t PIN_LDR_DIR = 34;       // sensor de linha direito  (saida digital)
const uint8_t LINHA_NIVEL_PRETO = HIGH; // leitura sobre o preto
const uint8_t PIN_LDR_TESTE_ESQ = A10; // LDR esquerdo (entrada analogica)
const uint8_t PIN_LDR_TESTE_DIR = A11; // LDR direito  (entrada analogica)
const uint8_t PIN_LDR_BAIXO     = 51;  // LDR inferior de luz (saida digital)
const uint8_t LUZ_NIVEL_ACESO   = LOW; // leitura sobre luz inferior acesa

// ======================= MOTORES =====================================
const uint8_t PORTA_MOT_ESQ = 1;       // M1
const uint8_t PORTA_MOT_DIR = 2;       // M2
const bool    INV_ESQ = false;         // Trocar sentido da roda
const bool    INV_DIR = false;

// ================== VELOCIDADES (0, 255) =============================
const int VEL_FRENTE = 55;    // andando reto
const int VEL_LENTA  = 45;    // aproximacao lenta perto de objeto
const int VEL_GIRO   = 70;    // velocidade dos giros
const int VEL_GIRO_ALINHA = 50; // velocidade do giro no alinhamento
const int VEL_RE     = 65;    // velocidade da re
const int VEL_MIN    = 40;    // velocidade minima
const int VEL_LINHA       = 50;  // segue-linha: roda externa ou andando reto
const int VEL_LINHA_LENTA = 20;  // segue-linha: roda interna desacelera na curva
const int VEL_LINHA_APROX = 35;  // segue-linha: velocidade de aproximacao
const int CURVA_LINHA_GRAUS = 80; // segue-linha: giro seguindo linha (quando ve dois pretos)

// Geometria / encoders -> Calculado empiricamente
const float COUNTS_PER_CM  = 1.99;    // 209 pulsos / 105 cm
const float COUNTS_PER_DEG = 0.2016;  // 127 pulsos / 630 graus
const float KP_RETO  = 3.0;   // Constante proporcional
const float KD_RETO  = 2.0;   // Constante derivativa

// Desliga os motores X pulsos antes do final
const long FOLGA_RETA = 6;
const long FOLGA_GIRO = 4;

// Tamanhos selecionaveis pelo LCD (cm) para a reta e o lado do quadrado
const uint8_t TAM_MIN   = 10;    // menor tamanho selecionavel (cm)
const uint8_t TAM_MAX   = 100;   // maior tamanho selecionavel (cm)
const uint8_t TAM_PASSO = 10;    // incremento ao ajustar pelos botoes (cm)

// Deteccao
const float DIST_DETECTA = 8.0;       // comeca a observar e desacelerar
const float DIST_COR_MAX = 3.0;       // Dist para parar e mostrar cor
const float DIST_PAREDE  = 2.0;       // Para nesta distancia de paredes

// ================== CLASSIFICACAO DE COR =============================
const uint16_t CLEAR_MIN       = 50;     // abaixo: escuro / sem objeto
const float    DIST_MAX        = 0.30;   // acima: cor nao reconhecida
const float    DIST_MAX_BRANCO = 0.010;  // Dif max entre branco lido e calibrado
const uint8_t  EEPROM_MAGIC = 0xC5; 

// ================== TEMPOS / LIMITES =================================
const unsigned long DEBOUNCE_US = 1500;  // so considera outro pulso apos 1,5 ms
const unsigned long SETTLE_MS   = 400;   // pausa entre passos
const unsigned long PAUSA_BLOCO_MS = 1500;  // mostra a cor do bloco antes de desviar
const unsigned long COOLDOWN_CANTO_MS = 1200; // segue-linha: trava novo giro de canto por X ms
#define MAX_PASSOS 12

// ======================= ENUMS COMPARTILHADOS ========================
// Cores
enum { COR_VERMELHO, COR_VERDE, COR_AZUL, COR_BRANCO };

enum { EST_MENU, EST_TAMANHO, EST_PASSOS, EST_LINHA, EST_COR, EST_CALIB, EST_TESTE_LDR, EST_ALINHA, EST_PARADO };

// Fases internas do alinhamento
enum { AL_AGUARDA, AL_GIRA, AL_ALINHA };

//Botoes
enum { BT_NENHUM, BT_DIR, BT_CIMA, BT_BAIXO, BT_ESQ, BT_SELECT };

// Tipos de trajeto
enum { TRAJ_RETA, TRAJ_QUADRADO };

// Tipos de movimento de baixo nivel
enum { MK_FRENTE, MK_RE, MK_GIRA_DIR, MK_GIRA_ESQ };

// Tipos de passo
enum { ST_FRENTE, ST_RE, ST_GIRA };

// ======================= TEXTOS (definidos em config.cpp) ============
extern const char* NOME_COR[4];
extern const char* ITENS_MENU[10];
