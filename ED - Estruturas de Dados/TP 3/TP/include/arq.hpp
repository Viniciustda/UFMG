#ifndef ARQ
#define ARQ

#include <iostream>
#include <string>

using namespace std;

class Arquivo { //fila -> dinamica  
    public:
        Arquivo();
        ~Arquivo();
    private:
        string conteudo;
        Arquivo *prox;
    friend class FilaEncadeada;
};

#endif