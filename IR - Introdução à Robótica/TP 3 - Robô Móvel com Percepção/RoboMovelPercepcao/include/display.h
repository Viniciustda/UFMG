#pragma once
/*
  display.h - LCD Keypad Shield (LiquidCrystal 8,9,4,5,6,7).
*/

#include <LiquidCrystal.h>

extern LiquidCrystal lcd;

void iniciaLCD();
void lcdLinha(uint8_t lin, const char* txt);
void atualizaLCD();
