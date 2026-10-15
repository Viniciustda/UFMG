#include "motores.h"
#include "config.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motEsq;
Adafruit_DCMotor *motDir;

void iniciaMotores() {
  AFMS.begin();  // Motorshield v2
  motEsq = AFMS.getMotor(PORTA_MOT_ESQ);
  motDir = AFMS.getMotor(PORTA_MOT_DIR);
  paraMotores();
}

// 0 = esquerdo, 1 = direito ; velocidade: (-255,255)
// Coloca velocidade no motor
void setMotor(uint8_t lado, int spd) {
  bool inv = (lado == 0) ? INV_ESQ : INV_DIR;
  Adafruit_DCMotor *m = (lado == 0) ? motEsq : motDir;
  if (inv) spd = -spd;
  if (spd >  255) spd =  255;
  if (spd < -255) spd = -255;
  if (spd > 0)      { m->setSpeed(spd);  m->run(FORWARD); }
  else if (spd < 0) { m->setSpeed(-spd); m->run(BACKWARD); }
  else              { m->setSpeed(0);    m->run(RELEASE); }
}
void paraMotores() {
  setMotor(0, 0);
  setMotor(1, 0);
}
