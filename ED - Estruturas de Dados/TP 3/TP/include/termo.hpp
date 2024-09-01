#ifndef TERMO
#define TERMO

#include "lista.hpp"
#include <iostream>
#include <string>

using namespace std;

class Termo { 
    public:
        Termo();
        ~Termo();
        string GetTermo();
        void SetTermo(string palavra);
        int GetFrequencia(int id);
        int GetIDPrimeiro();
        int GetProxID(int id);
        void IncrementaFrequencia(int id);
    private:
        string palavra;
        ListaEncadeada frequecia;
    friend void CriaIndice(string arq_consulta, string arq_ranking, string pasta_corpus, string arq_stopwords, int num_arq, int tam_vocabulario, Termo *indice);
    friend void Consulta(string arq_ranking, string arq_consulta, Termo indice[], int num_arq, int tam_vocabulario);
    friend int main(int argc, char *argv[]);
};

#endif