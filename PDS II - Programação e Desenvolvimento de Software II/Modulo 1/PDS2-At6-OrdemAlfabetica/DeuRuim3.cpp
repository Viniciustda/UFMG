//-------------hpp-------------


#include <iostream>
#include <iomanip>
#include <string>
#include <list>

using namespace std;

struct Texto {
    list<string> palavra;

    int repeticoes(int tamanho, list<string> Palavra, string word);
    float Percentual(int tamanho, list<string> Palavra, string word);
};

//list<string> OrdenaTexto(int tamanho, list<string> palavra);

//bool Repete(list<string> palavra);
//list<string> EliminaRepeticoes(int tamanho, list<string> palavra);


//------------cpp------------

#include "Indice.hpp"

int Texto::repeticoes(int tamanho, list<string> Palavra, string word) {
    list<string>::iterator it;
    int repeticao = 0;

    for (it = Palavra.begin(); it != Palavra.end(); ++it) {
        if (word == *it) {
            repeticao++;
        }   
    }
    return repeticao;
}

float Texto::Percentual(int tamanho, list<string> Palavra, string word) {
    
    float parcial;
    parcial = 1.0/tamanho;

    return repeticoes(tamanho, Palavra, word) * parcial;
}

/*list<string> OrdenaTexto(int tamanho, list<string> palavra) {

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
}*/

/*bool Repete(list<string> palavra) {
    bool resultado = 0;

    list<string>::iterator it;
    for (it = palavra.begin(); it != palavra.end(); ++it) {

        if (*it == *(it-1)) {
            resultado = 1;
        }
    }
    return resultado;
}*/

/*list<string> EliminaRepeticoes(int tamanho, list<string> palavra) {

    list<string>::iterator it;

    for (it = palavra.begin(); it != palavra.end(); ++it) {   
        (*it).remove_if();
    }

    return palavra;
}*/

/*list<string> EliminaRepeticoes(int tamanho, list<string> palavra) {

    for (int indice = 0; indice < tamanho; ++indice) {
        if (palavra[indice] == palavra[indice-1]) {
            palavra[indice].clear();
            
        }
    }

    return palavra;
}*/

/*list<string> EliminaRepeticoes(int tamanho, list<string> palavra) {

    int qtd = 0;

    for (int indice = 0; indice < tamanho; ++indice) {
        if (palavra[indice-1] == palavra[indice]) {
            for (int i = indice; i < tamanho; i++) {
                string aux = palavra[indice];
                palavra[indice] = palavra[indice+1];
                palavra[indice+1] = aux;
            }
            qtd++;
        }
    }

    for (int i = 0; i < qtd; i++) {
        palavra.pop_back();
    }
    
    return palavra;
}*/

//------------main------------

#include "Indice.hpp"

int main() {

    Texto text;
    string entrada;
    Texto semrepeticao;
    int tamanho = 0;
   
   while (std::cin >> entrada) {
      text.palavra.push_back(entrada);

      tamanho = text.palavra.size();

      for (int i = 0; i < tamanho; i++) {
         cout << text.palavra[i] << ' ' ;
         cout << text.repeticoes(tamanho, text.palavra, text.palavra[i]) << ' ' ;
         cout << fixed << setprecision(2) << text.Percentual(tamanho, text.palavra, text.palavra[i]) << endl;
      }
      cout << endl;

      text.palavra.sort();

      for (int i = 0; i < tamanho; i++) {
         cout << text.palavra[i] << ' ' ;
         cout << text.repeticoes(tamanho, text.palavra, text.palavra[i]) << ' ' ;
         cout << fixed << setprecision(2) << text.Percentual(tamanho, text.palavra, text.palavra[i]) << endl;
      }
      cout << endl;

      //semrepeticao.palavra = EliminaRepeticoes(tamanho, text.palavra);
      text.palavra.unique();

      for (int i = 0; i < tamanho; i++) {
         cout << text.palavra[i] << ' ' ;
         cout << text.repeticoes(tamanho, text.palavra, text.palavra[i]) << ' ' ;
         cout << fixed << setprecision(2) << text.Percentual(tamanho, text.palavra, text.palavra[i]) << endl;
      }
      cout << endl;


      /*for (int i = 0; i < tamanho; i++) {
         cout << semrepeticao.palavra[i] << ' ' ;
         cout << semrepeticao.repeticoes(tamanho, semrepeticao.palavra, semrepeticao.palavra[i]) << ' ' ;
         cout << fixed << setprecision(2) << semrepeticao.Percentual(tamanho, semrepeticao.palavra, semrepeticao.palavra[i]) << endl;
      }
      cout << endl;*/
   }
    
    return 0;    
}