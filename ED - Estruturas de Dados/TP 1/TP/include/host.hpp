#ifndef HOST
#define HOST

#include "lista.hpp"
#include <iostream>
#include <string>

using namespace std;

class Host { //fila -> dinamica  
    public:
        Host();
        ~Host();
    private:
        string conteudo;
        Host *prox;
        ListaEncadeada urls;  //cada host tem uma lista de url
    friend class FilaEncadeada;
    friend class Escalonador;
};

#endif