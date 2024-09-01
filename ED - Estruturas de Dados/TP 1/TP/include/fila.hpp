#ifndef FILA
#define FILA

#include "host.hpp"
#include <iostream>
#include <string>

using namespace std;

class Fila {
    public:
        Fila() {tamanho = 0;};
        int GetTamanho() {return tamanho;};
        bool Vazia() {return tamanho == 0;};
        virtual void Enfileira(string conteudo) = 0;
        virtual string Desenfileira() = 0;
        virtual void Limpa() = 0;
        virtual bool ConfereHost(string procurado) = 0;
    protected:
        int tamanho;
};

class FilaEncadeada : public Fila {
    public:
        FilaEncadeada();
        virtual ~FilaEncadeada();
        void Enfileira(string conteudo);
        string Desenfileira();
        void Limpa();
        bool ConfereHost(string procurado);
    private:
        Host* frente;
        Host* tras;
    friend class Escalonador;
};

#endif