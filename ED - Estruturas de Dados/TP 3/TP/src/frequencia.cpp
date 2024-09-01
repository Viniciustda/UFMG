#include "frequencia.hpp"
#include <iostream>

using namespace std;

Frequencia::Frequencia() {
    this->ID = -1;
    this->freq = 0; 
    this->prox = NULL;
}

Frequencia::~Frequencia() {}