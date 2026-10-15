#include <Servo.h>
#include <LiquidCrystal.h>
#include <math.h>

// CONSTANTES
const float L1 = 6.47;
const float L2 = 6.60;

const int PIN_SERVO1 = 25;
const int PIN_SERVO2 = 33;

// LCD Keypad Shield 
const int LCD_RS = 8, LCD_EN = 9, LCD_D4 = 4, LCD_D5 = 5, LCD_D6 = 6, LCD_D7 = 7;
const int PIN_BUTTONS = A0;

// Home: bracos a 90/90 -> ponta em (13.07, 0), totalmente esticado
const float HOME_X = 13.07;
const float HOME_Y = 0.0;

// GLOBAIS
Servo servo1, servo2;
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

const char* MENU_ITEMS[] = {"Quadrado", "Circunferencia", "Surpresinha :)"};
const int MENU_COUNT = 3;
int menuIndex = 0;

enum Button { BTN_NONE, BTN_RIGHT, BTN_UP, BTN_DOWN, BTN_LEFT, BTN_SELECT };

//  BOTOES 
Button readButton() {
    static Button lastBtn = BTN_NONE;
    int v = analogRead(PIN_BUTTONS);
    Button curr;
    if      (v < 50)  curr = BTN_RIGHT;
    else if (v < 200) curr = BTN_UP;
    else if (v < 400) curr = BTN_DOWN;
    else if (v < 600) curr = BTN_LEFT;
    else if (v < 800) curr = BTN_SELECT;
    else              curr = BTN_NONE;

    Button evt = BTN_NONE;
    if (curr != BTN_NONE && lastBtn == BTN_NONE) {
        delay(20);
        evt = curr;
    }
    lastBtn = curr;
    return evt;
}

//  CINEMATICA 
// theta2 = -arccos( (x*x + y*y - L1*L1 - L2*L2) / (2*L1*L2) )
// theta1 =  atan2(y, x) + arccos( (x*x + y*y + L1*L1 - L2*L2) / (2*L1*sqrt(x*x + y*y)) )
bool inverseKinematics(float x, float y, float &t1_deg, float &t2_deg) {
    float r2 = x*x + y*y;

    float c2 = (r2 - L1*L1 - L2*L2) / (2.0 * L1 * L2);
    if (c2 < -1.0 || c2 > 1.0) return false;
    float t2 = -acos(c2);

    float r = sqrt(r2);
    float c1 = (r2 + L1*L1 - L2*L2) / (2.0 * L1 * r);
    if (c1 < -1.0 || c1 > 1.0) return false;
    float t1 = atan2(y, x) + acos(c1);

    t1_deg = t1 * 180.0 / PI;
    t2_deg = t2 * 180.0 / PI;
    return true;
}

// Convencao: servo_angle = theta_deg + 90  (home 13.07,0 -> servos em 90/90).
void moveTo(float x, float y) {
    float t1, t2;
    if (!inverseKinematics(x, y, t1, t2)) return;
    int s1 = constrain((int)round(t1 + 90), 0, 180);
    int s2 = constrain((int)round(t2 + 90), 0, 180);
    servo1.write(s1);
    servo2.write(s2);
    Serial.print("x="); Serial.print(x); Serial.print(" y="); Serial.print(y);
    Serial.print(" t1="); Serial.print(t1); Serial.print(" t2="); Serial.print(t2);
    Serial.print(" s1="); Serial.print(s1); Serial.print(" s2="); Serial.println(s2);
}

void drawLine(float x0, float y0, float x1, float y1, int steps) {
    moveTo(x0, y0);
    
    for (int i = 1; i <= steps; i++) {
        float t = (float)i / steps;
        moveTo(x0 + t*(x1 - x0), y0 + t*(y1 - y0));
    }
    moveTo(x1, y1);
}

//  DESENHOS 
void drawSquare() {
    const float SQ_X0   = 9.5;
    const float SQ_Y0   = -1.5;
    const float SQ_SIDE = 3.0;

    const int STEPS_PER_SIDE = 10;

    lcd.clear();
    lcd.print("Desenhando");
    lcd.setCursor(0, 1);
    lcd.print("Quadrado 3cm...");

    moveTo(SQ_X0, SQ_Y0);
    
    drawLine(SQ_X0,           SQ_Y0,           SQ_X0 + SQ_SIDE, SQ_Y0,           STEPS_PER_SIDE); 
    drawLine(SQ_X0 + SQ_SIDE, SQ_Y0,           SQ_X0 + SQ_SIDE, SQ_Y0 + SQ_SIDE, STEPS_PER_SIDE); 
    drawLine(SQ_X0 + SQ_SIDE, SQ_Y0 + SQ_SIDE, SQ_X0,           SQ_Y0 + SQ_SIDE, STEPS_PER_SIDE); 
    drawLine(SQ_X0,           SQ_Y0 + SQ_SIDE, SQ_X0,           SQ_Y0,           STEPS_PER_SIDE); 
}

void drawCircle() {
    const float CX = 11.0;
    const float CY = 0.0;
    const float R  = 1.5;
    const int   POINTS = 240;

    lcd.clear();
    lcd.print("Desenhando");
    lcd.setCursor(0, 1);
    lcd.print("Circulo D=3cm");

    moveTo(CX + R, CY);

    for (int i = 1; i <= POINTS; i++) {
        float theta = 2.0 * PI * i / POINTS;
        moveTo(CX + R * cos(theta), CY + R * sin(theta));
    }
}

// Cubo
void drawExtra() {
    const float CX = 11.0;
    const float CY = 0.0;
    const float S  = 2.0;   // lado do cubo
    const float DX = 0.8;   // offset de perspectiva em X
    const float DY = 0.8;   // offset de perspectiva em Y
    const int   STEPS = 30;

    // Vertices da face frontal (A..D) e traseira (E..H)
    float Ax = CX - S/2, Ay = CY - S/2;    // frente inf-esq
    float Bx = CX + S/2, By = CY - S/2;    // frente inf-dir
    float Cx = CX + S/2, Cy = CY + S/2;    // frente sup-dir
    float Dx = CX - S/2, Dy = CY + S/2;    // frente sup-esq
    float Ex = Ax + DX,  Ey = Ay + DY;     // tras inf-esq
    float Fx = Bx + DX,  Fy = By + DY;     // tras inf-dir
    float Gx = Cx + DX,  Gy = Cy + DY;     // tras sup-dir
    float Hx = Dx + DX,  Hy = Dy + DY;     // tras sup-esq

    lcd.clear();
    lcd.print("Desenhando");
    lcd.setCursor(0, 1);
    lcd.print("Cubo");

    moveTo(Ax, Ay);
    delay(1500);

    drawLine(Ax, Ay,  Dx, Dy,  STEPS);  // A->D  
    drawLine(Dx, Dy,  Cx, Cy,  STEPS);  // D->H  
    drawLine(Cx, Cy,  Bx, By,  STEPS);  // C->B  
    drawLine(Bx, By,  Fx, Fy,  STEPS);  // B->F  
    drawLine(Fx, Fy,  Gx, Gy,  STEPS);  // F->G  
    drawLine(Gx, Gy,  Cx, Cy,  STEPS);  // G->C  
    drawLine(Cx, Cy,  Bx, By,  STEPS);  // C->B 
    drawLine(Bx, By,  Ax, Ay,  STEPS);  // B->A  
    drawLine(Ax, Ay,  Ex, Ey,  STEPS);  // A->E  
    drawLine(Ex, Ey,  Fx, Fy,  STEPS);  // E->F  
    drawLine(Fx, Fy,  Gx, Gy,  STEPS);  // F->G  
    drawLine(Gx, Gy,  Hx, Hy,  STEPS);  // G->H  
    drawLine(Hx, Hy,  Ex, Ey,  STEPS);  // H->E
    drawLine(Ex, Ey,  Fx, Fy,  STEPS);  // E->F
    drawLine(Fx, Fy,  Gx, Gy,  STEPS);  // F->G 
    drawLine(Gx, Gy,  Hx, Hy,  STEPS);  // G->H
    drawLine(Hx, Hy,  Dx, Dy,  STEPS);  // G->D

    delay(300);
}

//  MENU 
void showMenu() {
    lcd.clear();
    lcd.print("Selecione:");
    lcd.setCursor(0, 1);
    lcd.print(">");
    lcd.print(MENU_ITEMS[menuIndex]);
}

void executeMenu() {
    switch (menuIndex) {
        case 0: drawSquare(); drawSquare(); break;
        case 1: drawCircle(); drawCircle(); break;
        case 2: drawExtra(); break;
    }
    showMenu();
}

//  ARDUINO 
void setup() {
    Serial.begin(9600);

    servo1.attach(PIN_SERVO1);
    servo2.attach(PIN_SERVO2);
    moveTo(HOME_X, HOME_Y);

    lcd.begin(16, 2);
    lcd.print("Braco Artista");
    lcd.setCursor(0, 1);
    lcd.print("Iniciando...");
    delay(1200);

    showMenu();
}

void loop() {
    Button b = readButton();
    switch (b) {
        case BTN_UP:
            menuIndex = (menuIndex - 1 + MENU_COUNT) % MENU_COUNT;
            showMenu();
            break;
        case BTN_DOWN:
            menuIndex = (menuIndex + 1) % MENU_COUNT;
            showMenu();
            break;
        case BTN_SELECT:
            executeMenu();
            break;
        default:
            break;
    }
}