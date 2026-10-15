#include "botoes.h"
#include "config.h"

int leBotaoBruto() {
  int v = analogRead(PIN_BOTOES);
  if (v <  60) return BT_DIR;
  if (v < 250) return BT_CIMA;
  if (v < 450) return BT_BAIXO;
  if (v < 650) return BT_ESQ;
  if (v < 850) return BT_SELECT;
  return BT_NENHUM;
}

// Botoes estavam com mal contato: so retorna na transicao e espera quando aperta
int eventoBotao() {
  static int anterior = BT_NENHUM;
  static unsigned long tUltimo = 0;
  int atual = leBotaoBruto();
  int ev = BT_NENHUM;
  if (atual != BT_NENHUM && anterior == BT_NENHUM &&
      millis() - tUltimo > 300) {
    ev = atual;
    tUltimo = millis();
  }
  anterior = atual;
  return ev;
}
