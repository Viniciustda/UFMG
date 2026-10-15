#include "modos.h"
#include "config.h"
#include "motores.h"
#include "encoders.h"
#include "movimento.h"
#include "passos.h"
#include "sensores.h"
#include "armazenamento.h"
#include <stdio.h>
#include <string.h>

uint8_t estado = EST_MENU;
uint8_t menuIdx   = 0;
uint8_t trajTipo  = TRAJ_RETA;
uint8_t tamCm     = 20;
uint8_t calibIdx  = 0;
uint8_t alFase    = AL_AGUARDA;
char msgParado[17] = "";

// O que fazer quando o alinhamento terminar (reta/quadrado/linha ou nada)
uint8_t acaoAposAlinhar = ACAO_NENHUMA;
bool    alinhaAntes     = false;

// Reacao a bloco: mostra a cor por um tempo e entao desvia
bool          reagindoBloco = false;
unsigned long tBloco = 0;
char          msgBloco[17] = "";

// Executa a lista de passos
void atualizaPassos() {
  // Pausa de reacao: mostra a cor do bloco e depois desvia
  if (reagindoBloco) {
    paraMotores();
    if (millis() - tBloco >= PAUSA_BLOCO_MS) {
      reagindoBloco = false;
      reiniciaTrajetoAposBloco(trajTipo, tamCm);   // gira p/ direita e recomeca
    }
    return;
  }

  bool emMovimento = atualizaMovimento();

  if (emMovimento) {
    if (passoTipo[passoAtual] == ST_FRENTE) {
      uint8_t ev = monitoraSensores();
      // desacelera ao se aproximar (entre 3,0 e 8 cm)
      velFrenteAtual = (leituraDistAtual > DIST_COR_MAX && leituraDistAtual <= DIST_DETECTA) ? VEL_LENTA : VEL_FRENTE;
      if (ev == 2) {  // bloco: mostra a cor, pausa e desvia
        paraMotores();
        movAtivo = false;
        Serial.print(F("Bloco detectado: "));
        Serial.println(NOME_COR[corDetectada]);
        snprintf(msgBloco, sizeof(msgBloco), "Bloco: %s", NOME_COR[corDetectada]);
        reagindoBloco = true;
        tBloco = millis();
      } else if (ev == 1) {  // parede
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
  strcpy(msgParado, "Trajeto OK");
  estado = EST_PARADO;
}

// Selecao de tamanho antes de iniciar Reta ou Quadrado
// CIMA/BAIXO ajustam de 10 em 10 cm, entre TAM_MIN e TAM_MAX
void atualizaTamanho(int botao) {
  paraMotores();
  if (botao == BT_CIMA  && tamCm < TAM_MAX) tamCm += TAM_PASSO;
  if (botao == BT_BAIXO && tamCm > TAM_MIN) tamCm -= TAM_PASSO;
  if (botao == BT_ESQ) { estado = EST_MENU; return; }   // cancela
  if (botao == BT_SELECT) {
    if (alinhaAntes) {
      // alinha primeiro e carrega o trajeto
      acaoAposAlinhar = ACAO_PASSOS;
      iniciaTestaAlinhamento();
      estado = EST_ALINHA;
    } else {
      carregaTrajeto(trajTipo, tamCm);
      estado = EST_PASSOS;
    }
  }
}

// Segue uma linha preta com os dois LDRs frontais
// Se um lado ve preto, curva suave para esse lado (roda interna desacelera)
// Canto de 90 graus: quando os DOIS sensores veem preto, gira na direcao do sensor que viu preto primeiro
// Bloco a frente: para, mostra a cor por PAUSA_BLOCO_MS e gira 180 graus
void atualizaSegueLinha() {
  static bool girando = false;
  static int  primeiroLado = 0;        // 0 nenhum, 1 esquerda, 2 direita
  static unsigned long tFimGiro = 0;   // instante em que o ultimo giro terminou

  // Reagindo a bloco: para, mostra a cor e depois gira 180 graus
  if (reagindoBloco) {
    paraMotores();
    if (millis() - tBloco >= PAUSA_BLOCO_MS) {
      reagindoBloco = false;
      velGiroAtual = VEL_GIRO;
      iniciaMovimento(MK_GIRA_DIR, (long)(180.0 * COUNTS_PER_DEG) - FOLGA_GIRO);
      girando = true;
    }
    return;
  }

  // Em curva (canto de 80 ou desvio de 180): executa o giro ate concluir
  if (girando) {
    if (!atualizaMovimento()) {
      girando = false;
      primeiroLado = 0;
      tFimGiro = millis(); // Evitar girar varias vezes rapido
    }
    return;
  }

  // Bloco a frente: dispara a reacao (mostra cor 1,5s e depois gira 180)
  if (monitoraSensores() == 2) {
    paraMotores();
    Serial.print(F("Bloco detectado: "));
    Serial.println(NOME_COR[corDetectada]);
    snprintf(msgBloco, sizeof(msgBloco), "Bloco: %s", NOME_COR[corDetectada]);
    reagindoBloco = true;
    tBloco = millis();
    return;
  }

  bool pe = linhaEsqPreto();
  bool pd = linhaDirPreto();

  // Desacelera ao aproximar de objetos
  int vBase = (leituraDistAtual > DIST_COR_MAX && leituraDistAtual <= DIST_DETECTA) ? VEL_LINHA_APROX : VEL_LINHA;

  // Tempo de protecao: evita girar varias vezes seguidas
  bool emCooldown = (millis() - tFimGiro < COOLDOWN_CANTO_MS);

  if (pe && pd && !emCooldown) {  // canto: dois pretos -> gira p/ o 1o preto visto
    int lado = (primeiroLado != 0) ? primeiroLado : 1;   // empate: assume esquerda
    velGiroAtual = VEL_GIRO;
    iniciaMovimento(lado == 1 ? MK_GIRA_ESQ : MK_GIRA_DIR,
                    (long)(CURVA_LINHA_GRAUS * COUNTS_PER_DEG) - FOLGA_GIRO);
    girando = true;
  } else if (pe && !pd) {  // linha sob o sensor esquerdo: curva p/ esquerda
    if (primeiroLado == 0) primeiroLado = 1;
    setMotor(0, VEL_LINHA_LENTA);
    setMotor(1, vBase);
  } else if (pd && !pe) {   // curva p/ direita
    if (primeiroLado == 0) primeiroLado = 2;
    setMotor(0, vBase);
    setMotor(1, VEL_LINHA_LENTA);
  } else {                      // ambos branco: segue reto
    primeiroLado = 0;
    setMotor(0, vBase);
    setMotor(1, vBase);
  }
}

// Modo: parado le os dois LDRs de alinhamento e mostra no lcd
void atualizaTesteLdr() {
  paraMotores();
}

// Alinhamento: robo gira 360 e salva posicao que identifica o minimo nos ldrs superiores
// Alinha 180 graus relativo a posicao salva
static long alMinValor;   // menor min(esq,dir) visto no giro
static long alMinProg;    // orientacao (pulsos) onde ocorreu esse minimo

// Encerra o alinhamento: comeca a acao pedida (reta/quadrado/linha) ou para
static void finalizaAlinhamento() {
  switch (acaoAposAlinhar) {
    case ACAO_PASSOS:
      carregaTrajeto(trajTipo, tamCm);   // recarrega e zera encoders via iniciaMovimento
      estado = EST_PASSOS;
      break;
    case ACAO_LINHA:
      estado = EST_LINHA;
      break;
    default:                             // funcao 7 isolada: fica parado
      estado = EST_PARADO;
      break;
  }
  acaoAposAlinhar = ACAO_NENHUMA;        // reset para a proxima vez
}

void iniciaTestaAlinhamento() {
  paraMotores();
  movAtivo = false;
  velGiroAtual = VEL_GIRO_ALINHA;   // giro mais lento para ler direito
  alFase   = AL_AGUARDA;
  alMinValor = 2000;
  alMinProg  = 0;
}

void atualizaTestaAlinhamento() {
  switch (alFase) {
    case AL_AGUARDA: {                 // espera a luz apagar para comecar
      paraMotores();
      static bool viuLuz = false;
      bool acesa = luzAcesa();
      if (acesa) viuLuz = true;        // so dispara depois de ver a luz acesa
      if (viuLuz && !acesa) {
        viuLuz = false;
        alMinValor = 2000;
        alMinProg  = 0;
        iniciaMovimento(MK_GIRA_DIR, (long)(360.0 * COUNTS_PER_DEG));
        alFase = AL_GIRA;
      }
      break;
    }
    case AL_GIRA: {                    // gira 360 amostrando os LDRs superiores
      bool emMov = atualizaMovimento();
      if (emMov) {
        int e = leLdrTesteEsq();
        int d = leLdrTesteDir();
        long m = (e < d) ? e : d;
        long prog = (lerEncEsq() + lerEncDir()) / 2;
        if (m < alMinValor) { alMinValor = m; alMinProg = prog; }
      } else {
        if (millis() - tParou < SETTLE_MS) { paraMotores(); break; }
        // Termina 180 graus oposto ao minimo: giro = 180 - angulo_do_minimo
        long delta = (long)(180.0 * COUNTS_PER_DEG) - alMinProg;   // em pulsos
        Serial.print(F("Alinhamento: min="));
        Serial.print(alMinValor);
        Serial.print(F(" em ")); Serial.print((int)(alMinProg / COUNTS_PER_DEG));
        Serial.print(F("g; giro de ")); Serial.print((int)(delta / COUNTS_PER_DEG));
        Serial.println(F("g ate o oposto"));
        if (delta == 0) {              // minimo a 180g: ja esta no oposto
          strcpy(msgParado, "Oposto OK");
          finalizaAlinhamento();
        } else {
          // delta > 0: gira no sentido oposto ao do giro (MK_GIRA_ESQ)
          // delta < 0: gira no mesmo sentido do giro (MK_GIRA_DIR)
          uint8_t sentido = (delta > 0) ? MK_GIRA_ESQ : MK_GIRA_DIR;
          long counts = (delta > 0) ? delta : -delta;
          iniciaMovimento(sentido, counts);
          alFase = AL_ALINHA;
        }
      }
      break;
    }
    case AL_ALINHA: {                  // gira ate a orientacao oposta ao minimo
      if (!atualizaMovimento()) {
        int ori = ((int)(alMinProg / COUNTS_PER_DEG) + 180) % 360;
        snprintf(msgParado, sizeof(msgParado), "Oposto min %dg", ori);
        finalizaAlinhamento();
      }
      break;
    }
  }
}

// Modo: parado, apenas identifica a cor na frente
void atualizaCor() {
  paraMotores();
  static unsigned long ultimo = 0;
  if (millis() - ultimo > 250) {
    ultimo = millis();
    float d = leDistanciaCm();
    leituraDistAtual = (d < 0) ? -1 : (int)d;
    // so identifica se o objeto estiver perto o bastante (<= DIST_COR_MAX)
    if (d > 0 && d <= DIST_COR_MAX) corVistaAtual = classificaCor(4);
    else                            corVistaAtual = -2;   // -2 = aproxima mais
  }
}

// Modo 6: calibracao das cores
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

  // Botoes da LCD tavam ruins, aqui ele so pega botao a cada 600ms
  static unsigned long tUltimaCaptura = 0;
  if (millis() - tUltimaCaptura < 600) return;
  tUltimaCaptura = millis();

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
  if (calibIdx >= 4) {                          // 3 cores + branco prontos
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
  if (botao == BT_BAIXO && menuIdx < 9) menuIdx++;
  if (botao == BT_SELECT) {
    if (menuIdx == 0)      { trajTipo = TRAJ_RETA;     alinhaAntes = true;  estado = EST_TAMANHO; }
    else if (menuIdx == 1) { trajTipo = TRAJ_QUADRADO; alinhaAntes = true;  estado = EST_TAMANHO; }
    else if (menuIdx == 2) { acaoAposAlinhar = ACAO_LINHA; iniciaTestaAlinhamento(); estado = EST_ALINHA; }
    else if (menuIdx == 3) { corVistaAtual = -1; estado = EST_COR; }
    else if (menuIdx == 4) { entraCalib(); }
    else if (menuIdx == 5) { estado = EST_TESTE_LDR; }
    else if (menuIdx == 6) { acaoAposAlinhar = ACAO_NENHUMA; iniciaTestaAlinhamento(); estado = EST_ALINHA; }
    else if (menuIdx == 7) { trajTipo = TRAJ_RETA;     alinhaAntes = false; estado = EST_TAMANHO; }
    else if (menuIdx == 8) { trajTipo = TRAJ_QUADRADO; alinhaAntes = false; estado = EST_TAMANHO; }
    else                   { estado = EST_LINHA; }   // 10: segue linha sem alinhar
  }
}

void entraCalib() {
  calibIdx = 0;
  estado = EST_CALIB;
  Serial.println(F("--- Calibracao de cor ---"));
  Serial.println(F("Mostre cada cor a ~4 cm e aperte SELECT (LCD) ou qualquer tecla (PC)."));
  Serial.println(F("Sao 4 capturas: VERMELHO, VERDE, AZUL e a PAREDE branca."));
  Serial.print(F("Agora mostre: "));
  Serial.println(NOME_COR[0]);
}

// Atalho de calibracao pelo PC (Serial): 'c'
void calibraPelaSerial() {
  if (estado != EST_MENU) return;
  if (!Serial.available()) return;
  char c = Serial.read();
  if (c == 'c' || c == 'C') entraCalib();
}
