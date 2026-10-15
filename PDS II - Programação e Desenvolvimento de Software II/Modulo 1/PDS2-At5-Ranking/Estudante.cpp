#include "Estudante.hpp"

float Estudante::calcularRSG() {
    float RSG = 0.0;

    for (int i = 0; i < 5; i++)
        RSG += notas[i];
    RSG /= 5.0;

    return RSG;
}