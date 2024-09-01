//------------ hpp ------------

#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Indice {
    std::string palavra;
    int repeticoes = 0;

    float Percentual(int TamanhoTexto);
};

//------------ cpp ------------

#include "Indice.hpp"

float Indice::Percentual(int TamanhoTexto) {
    float parcial;
    parcial = 1.0/TamanhoTexto;

    return repeticoes * parcial;
}

//------------ main ------------

#include "Indice.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

int main(){
    vector<Indice> texto;
    vector<Indice> textoSemRepeticao;
    Indice p;

    while (cin >> p.palavra) {
        p.repeticoes = 0;
        cin >> p.palavra;
        for (int i = 0; i < texto.size(); i++) {
            if (p.palavra == texto[i].palavra) {
                texto[i].repeticoes ++;
                p.repeticoes ++;
            }
        }
        texto.push_back(p);
    }

    /*
    while (cin >> palavra) {
        // seu código
    }
    */

    for (int indice = 0; indice < texto.size(); ++indice) {
        int indiceMenor = indice;
        for (int indiceSeguinte = indice+1; indiceSeguinte < texto.size(); ++indiceSeguinte) {
            if (texto[indiceSeguinte].palavra < texto[indiceMenor].palavra) {
                indiceMenor = indiceSeguinte;
            }
        }
        Indice aux = texto[indice];
        texto[indice] = texto[indiceMenor];
        texto[indiceMenor] = aux;
    }
    /*
    for (int i = 0; i < texto.size(); i++) {
        for (int j = 0; j < textoSemRepeticao.size(); j++) {
            if (texto[i].palavra == textoSemRepeticao[j].palavra) {
                break;
            }
            else { 
                textoSemRepeticao.push_back(texto);
            }
            
        }
    }
    */

   for (int i = 0; i < texto.size(); i++) {
       cout << texto[i].palavra << ' ' ;
       cout << texto[i].repeticoes << ' ' ;
       cout << fixed << setprecision(2) << texto[i].Percentual(texto.size()) << endl;
   }
   

    return 0;
}