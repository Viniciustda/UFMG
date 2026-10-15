#include "armazenamento.h"
#include "config.h"
#include "sensores.h"  
#include <EEPROM.h>

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
