//------------hpp------------

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

struct Texto {
    vector<string> palavra;

    int repeticoes(int tamanho, vector<string> Palavra, string word);
    float Percentual(int tamanho, vector<string> Palavra, string word);
};

vector<string> OrdenaTexto(int tamanho, vector<string> palavra);

vector<string> EliminaRepeticoes(int tamanho, vector<string> palavra);

//------------cpp------------

#include "Indice.hpp"

int Texto::repeticoes(int tamanho, vector<string> Palavra, string word) {
    int repeticao = 0;

    for (int i = 0; i < tamanho; i++) {
        if (word == Palavra[i]) {
            repeticao++;
        }   
    }
    return repeticao;
}

float Texto::Percentual(int tamanho, vector<string> Palavra, string word) {
    
    float parcial;
    parcial = 1.0/tamanho;

    return repeticoes(tamanho, Palavra, word) * parcial;
}

vector<string> OrdenaTexto(int tamanho, vector<string> palavra) {

    for (int indice = 0; indice < tamanho; ++indice) {
        int indiceMenor = indice;
        for (int indiceSeguinte = indice+1; indiceSeguinte < tamanho; ++indiceSeguinte) {
            if (palavra[indiceSeguinte] < palavra[indiceMenor]) {
                indiceMenor = indiceSeguinte;
            }
        }
        string aux = palavra[indice];
        palavra[indice] = palavra[indiceMenor];
        palavra[indiceMenor] = aux;
    }

    return palavra;
}

vector<string> EliminaRepeticoes(int tamanho, vector<string> palavra) {

    for (int indice = 0; indice < tamanho; ++indice) {
        if (palavra[indice] == palavra[indice-1]) {
            palavra.erase (palavra.begin()+indice);
            
        }
    }

    return palavra;
}

//------------main------------

#include "Indice.hpp"

int main() {

    Texto text;
    string entrada;
    Texto semrepeticao;
    int tamanho = 0;
    
    /*while (cin >> entrada) {
        cin >> entrada;
        text.palavra.push_back(entrada);
    }*/

   
   while (std::cin >> entrada) {
      text.palavra.push_back(entrada);

      tamanho = text.palavra.size();

      for (int i = 0; i < tamanho; i++) {
         cout << text.palavra[i] << ' ' ;
         cout << text.repeticoes(tamanho, text.palavra, text.palavra[i]) << ' ' ;
         cout << fixed << setprecision(2) << text.Percentual(tamanho, text.palavra, text.palavra[i]) << endl;
      }
      cout << endl;

      text.palavra = OrdenaTexto(tamanho, text.palavra);

      for (int i = 0; i < tamanho; i++) {
         cout << text.palavra[i] << ' ' ;
         cout << text.repeticoes(tamanho, text.palavra, text.palavra[i]) << ' ' ;
         cout << fixed << setprecision(2) << text.Percentual(tamanho, text.palavra, text.palavra[i]) << endl;
      }
      cout << endl;

      semrepeticao.palavra = EliminaRepeticoes(tamanho, text.palavra);

      for (int i = 0; i < tamanho; i++) {
         cout << semrepeticao.palavra[i] << ' ' ;
         cout << semrepeticao.repeticoes(tamanho, semrepeticao.palavra, semrepeticao.palavra[i]) << ' ' ;
         cout << fixed << setprecision(2) << semrepeticao.Percentual(tamanho, semrepeticao.palavra, semrepeticao.palavra[i]) << endl;
      }
      cout << endl;
   }
    
    return 0;    
}