#ifndef M_PI
#define PI  3.14
#endif

#ifndef CIRCUNFERENCIA_H
#define CIRCUNFERENCIA_H

#include <cmath>

struct Circunferencia {
    double _x, _y;
    double _raio;
    
    Circunferencia(double, double, double);
    double calcularArea();
    bool possuiIntersecao(Circunferencia* circ);
};
    
#endif