#include "Circunferencia.hpp"

Circunferencia::Circunferencia(double x, double y, double raio) {
    _x = x;
    _y = y;
    _raio= raio;
}

double Circunferencia::calcularArea() {
    return PI * pow(_raio, 2);
}

bool Circunferencia::possuiIntersecao(Circunferencia* circ) {
    double somaRaios = 0;
    double distEixosX = 0;
    double distEixosY = 0;

    somaRaios = _raio + circ->_raio;

    if (_x >= circ->_x) {
        distEixosX = _x - circ->_x;
    } else {
        distEixosX = circ->_x - _x;
    }
    if (_y >= circ->_y) {
        distEixosY = _y - circ->_y;
    } else {
        distEixosY = circ->_y - _y;
    }
    
    

    if (distEixosX <= somaRaios && distEixosY <= somaRaios) {
        return true;
    }else {
        return false;
    }
}