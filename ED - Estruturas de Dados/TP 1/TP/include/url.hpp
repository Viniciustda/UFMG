#ifndef URL
#define URL

#include <iostream>
#include <string>

using namespace std;

class Url { //lista -> dinamica
    public:
        Url();
    private:
        int num_barras;  //usado para saber em qual posicao a url vai entrar
        string conteudo;
        Url *prox;
    friend class ListaEncadeada;
};

#endif