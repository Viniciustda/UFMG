#ifndef LISTA
#define LISTA

#include "frequencia.hpp"
#include "arq_funcao.hpp"
#include <iostream>
#include <string>

using namespace std;

class Lista {
    public:
        Lista() {tamanho = 0;};
        int GetTamanho() {return tamanho;};
        bool Vazia() {return tamanho == 0;};
        virtual int GetFrequencia(int id) = 0;
        virtual void IncrementaFrequencia(int id) = 0;
        virtual void Insere(int id) = 0;
        virtual void Limpa() = 0;
        virtual bool PesquisaID(int id) = 0;
    protected:
        int tamanho;
};

class ListaEncadeada : public Lista {
    public:
        ListaEncadeada();
        ~ListaEncadeada();
        int GetFrequencia(int id);
        int GetIDPrimeiro();
        int GetProxID(int id);
        void IncrementaFrequencia(int id);
        void Insere(int id);
        void Limpa();
        bool PesquisaID(int id);
    private:
        Frequencia* primeiro;
        Frequencia* ultimo;
        Frequencia* Posiciona(int id, bool antes);
};

#endif