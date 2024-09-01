#ifndef ESCALONADOR
#define ESCALONADOR

#include "url.hpp"
#include "host.hpp"
#include "lista.hpp"
#include "fila.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class Escalonador {
    public:
        Escalonador();
        ~Escalonador();
        void ADD_URLS(string entrada_url);              //  ADD_URLS: adiciona ao escalonador a URL.
        void ESCALONA_TUDO();                           //  ESCALONA_TUDO: escalona todas as URLs seguindo as regras estabelecidas previamente. Quando escalonadas, as URLs são exibidas e removidas da lista.
        void ESCALONA(int n);                           //  ESCALONA <quantidade>: limita a quantidade de URLs escalonadas.
        void ESCALONA_HOST(string host, int n);         //  ESCALONA_HOST <host> <quantidade>: são escalonadas apenas uma quantidade especificada de URLs somente deste host.
        void VER_HOST(string host);                     //  VER_HOST <host>: exibe todas as URLs do host, na ordem de prioridade(numero de barras determina a prioridade).
        void LISTA_HOSTS();                             //  LISTA_HOSTS: exibe todos os hosts, seguindo a ordem em que foram conhecidos.
        void LIMPA_HOST(string host);                   //  LIMPA_HOST <host>: limpa a lista de URLs do host.
        void LIMPA_TUDO();                              //  LIMPA_TUDO: limpa todas as URLs, inclusive os hosts.
        void NomeArqSaida(string nome);

    private:
        FilaEncadeada hosts;
        string nome_arq_saida;                          //  Grava o nome do arquivo de saida

};

#endif