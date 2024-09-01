#ifndef FILAENCADEADA
#define FILAENCADEADA

#include "arq.hpp"
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
    private:
        Arquivo* frente;
        Arquivo* tras;
};

#endif