#include "display.h"
#include "config.h"
#include "sensores.h"
#include "passos.h"
#include "modos.h"
#include <stdio.h>
#include <string.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void iniciaLCD() {
  lcd.begin(16, 2);
  lcd.print("Gambiarra Tech");
}

// escreve um texto ocupando 16 colunas
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
      strcpy(l0, "TP3  Selecione:");
      snprintf(l1, sizeof(l1), ">%s", ITENS_MENU[menuIdx]);
      break;
    case EST_TAMANHO:
      strcpy(l0, trajTipo == TRAJ_RETA ? "Reta tamanho:" : "Quadrado lado:");
      snprintf(l1, sizeof(l1), ">%d cm  SEL inicia", tamCm);
      break;
    case EST_PASSOS:
      if (reagindoBloco) {
        strcpy(l0, msgBloco);
        strcpy(l1, "Desviando...");
      } else {
        snprintf(l0, sizeof(l0), "%s %dcm",
                 trajTipo == TRAJ_RETA ? "Reta" : "Quadrado", tamCm);
        snprintf(l1, sizeof(l1), "Passo %d/%d d:%s",
                 passoAtual + 1, numPassos, db);
      }
      break;
    case EST_LINHA:
      if (reagindoBloco) {
        strcpy(l0, msgBloco);
        strcpy(l1, "Girando 180...");
      } else {
        strcpy(l0, "Segue linha");
        snprintf(l1, sizeof(l1), "Esq:%d  Dir:%d", linhaEsqPreto(), linhaDirPreto());
      }
      break;
    case EST_TESTE_LDR:
      strcpy(l0, "Teste LDR");
      snprintf(l1, sizeof(l1), "E:%-4d D:%-4d", leLdrTesteEsq(), leLdrTesteDir());
      break;
    case EST_ALINHA:
      strcpy(l0, acaoAposAlinhar == ACAO_NENHUMA ? "Alinhamento" : "Alinha+exec");
      if (alFase == AL_AGUARDA)
        snprintf(l1, sizeof(l1), "Aguarda luz L:%d", luzAcesa());
      else if (alFase == AL_GIRA)
        strcpy(l1, "Girando 360...");
      else
        strcpy(l1, "Alinhando...");
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
