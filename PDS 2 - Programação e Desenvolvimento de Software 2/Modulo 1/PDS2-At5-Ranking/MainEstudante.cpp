#include "Estudante.hpp"

#include <iostream>
#include <iomanip>

using namespace std;

int main() {

    Estudante estudantes[10];

    for (int i = 0; i < 10; i++) {
        cin >> estudantes[i].matricula;
        cin >> estudantes[i].nome;
        for (int j = 0; j < 5; j++) {
            cin >> estudantes[i].notas[j];
        }
    }
    
    for (int indice = 0; indice < 10; ++indice) {
        int indiceMaior = indice;
        for (int indiceSeguinte = indice+1; indiceSeguinte < 10; ++indiceSeguinte) {
            if (estudantes[indiceSeguinte].calcularRSG() > estudantes[indiceMaior].calcularRSG()) {
                indiceMaior = indiceSeguinte;
            }
            if (estudantes[indiceSeguinte].calcularRSG() == estudantes[indiceMaior].calcularRSG()) {
                if (estudantes[indiceSeguinte].matricula < estudantes[indiceMaior].matricula) {
                    indiceMaior = indiceSeguinte;
                }
            }
        }
        Estudante aux = estudantes[indice];
        estudantes[indice] = estudantes[indiceMaior];
        estudantes[indiceMaior] = aux;
    }

    for (int i = 0; i < 3; i++) {
        cout << estudantes[i].matricula << ' ';
        cout << estudantes[i].nome << ' ';
        cout << fixed << setprecision(2) << estudantes[i].calcularRSG() << endl;
    }

    return 0;
}