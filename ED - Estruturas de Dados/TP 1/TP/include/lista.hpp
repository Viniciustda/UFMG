#ifndef LISTA
#define LISTA

#include "url.hpp"
#include "funcao_arq.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Lista {
    public:
        Lista() {tamanho = 0;};
        int GetTamanho() {return tamanho;};
        bool Vazia() {return tamanho == 0;};
        virtual string GetConteudo(int pos) = 0;
        virtual void SetConteudo(string conteudo, int pos) = 0;
        virtual void InsereInicio(string conteudo, int n_barras) = 0;
        virtual void InsereFinal(string conteudo, int n_barras) = 0;
        virtual void InserePosicao(string conteudo, int n_barras) = 0;
        virtual string RemoveInicio() = 0;
        virtual string RemoveFinal() = 0;
        virtual string RemovePosicao(int pos) = 0;
        virtual void Imprime(string nome_arq_saida) = 0;
        virtual void Imprime(string nome_arq_saida, int n) = 0;
        virtual void Limpa() = 0;
        virtual void Limpa(int n) = 0;
        virtual bool ConfereUrl(string procurado) = 0;
    protected:
        int tamanho;
};

class ListaEncadeada : public Lista {
    public:
        ListaEncadeada();
        ~ListaEncadeada();
        string GetConteudo(int pos);
        void SetConteudo(string conteudo, int pos);
        void InsereInicio(string conteudo, int n_barras);
        void InsereFinal(string conteudo, int n_barras);
        void InserePosicao(string conteudo, int n_barras);
        string RemoveInicio();
        string RemoveFinal();
        string RemovePosicao(int pos);
        void Imprime(string nome_arq_saida);
        void Imprime(string nome_arq_saida, int n);
        void Limpa();
        void Limpa(int n);
        bool ConfereUrl(string procurado);
    private:
        Url* primeiro;
        Url* ultimo;
        Url* Posiciona(int n_barras, bool antes);
};

#endif