#pragma once
/*
  motores.h - Adafruit Motor Shield V2 (motor esq = M1, motor dir = M2).
*/

#include <Adafruit_MotorShield.h>

extern Adafruit_MotorShield AFMS;
extern Adafruit_DCMotor *motEsq;
extern Adafruit_DCMotor *motDir;

void iniciaMotores();                  // AFMS.begin() + getMotor + para
void setMotor(uint8_t lado, int spd);  // 0 = esq, 1 = dir ; spd (-255,255)
void paraMotores();
