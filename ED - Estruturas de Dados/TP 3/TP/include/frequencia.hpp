#ifndef FREQUENCIA
#define FREQUENCIA

#include <iostream>

using namespace std;

class Frequencia { 
    public:
        Frequencia();
        ~Frequencia();

    private:
        int ID;
        int freq;
        Frequencia *prox;
    friend class ListaEncadeada;
    
};

#endif