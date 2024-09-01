#include "url.hpp"
#include "host.hpp"
#include "lista.hpp"
#include "fila.hpp"
#include "escalonador.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Escalonador::Escalonador() {
    nome_arq_saida = "sem_nome";
}

Escalonador::~Escalonador() {}

//  ADD_URLS: adiciona ao escalonador a URL.
void Escalonador::ADD_URLS(string entrada_url) {

    string entrada_host;
    string aux;
    int indice = 0;
    int n_barras = -2; //comeca com -2 porque n conta as duas barras do http://
    int tam = 0;
    Host *atual;

    indice = entrada_url.length() - 4;
    for (int i = 0; i < 4; i++)
        aux += entrada_url[indice+i]; 

    if (!entrada_url.find_first_of("http://") && aux!=".jpg" && aux!=".gif" && aux!=".mp3" && aux!=".avi" && aux!=".doc" && aux!=".pdf") { //confere se eh http e HTML, se nao for, nao faz nada.
        
        //separa o host da entrada
        entrada_host = entrada_url;
        if (entrada_host[7] == 'w' && entrada_host[8] == 'w' && entrada_host[9] == 'w' && entrada_host[10] == '.') 
            entrada_host.erase(0,11);
        else 
            entrada_host.erase(0,7);
        indice = entrada_host.find_first_of('/',0);
        entrada_host.erase(indice,entrada_host.length()-indice);

        //separa a url da entrada
        if (entrada_url[7] == 'w' && entrada_url[8] == 'w' && entrada_url[9] == 'w' && entrada_url[10] == '.') 
            entrada_url.erase(7,4);
        indice = entrada_url.find_first_of('#',0);
        if (indice != -1)
            entrada_url.erase(indice,entrada_url.length()-indice);
        if (entrada_url.back() == '/') 
            entrada_url.pop_back();

        //conta o numero de barras da url
        tam = entrada_url.length();
        for (int i = 0; i < tam; i++) {
            if (entrada_url[i] == '/') 
                n_barras++;
        }

        //confere se ja existe esse host 
        if (hosts.ConfereHost(entrada_host)) {

            atual = hosts.frente;

            while (atual->conteudo != entrada_host)
                atual = atual->prox;
            
            if (!atual->urls.ConfereUrl(entrada_url)) //confere se ja existe essa url no host. se sim, nao insere. se nao, insere
                atual->urls.InserePosicao(entrada_url,n_barras);
            
        }
        else { //se nao existe esse host, adiciona o host e enfileira a url
            hosts.Enfileira(entrada_host);
            hosts.tras->urls.InsereInicio(entrada_url,n_barras);
        }
    }
}





//  ESCALONA_TUDO: escalona todas as URLs seguindo as regras estabelecidas previamente.
//  Quando escalonadas, as URLs são exibidas e removidas da lista.
void Escalonador::ESCALONA_TUDO() {
    Host *atual;
    atual = hosts.frente;
    for (int i = 0; i <= hosts.tamanho; i++) {
        atual->urls.Imprime(this->nome_arq_saida);
        atual->urls.Limpa();
        atual = atual->prox;
    }
}





//  ESCALONA <quantidade>: limita a quantidade de URLs escalonadas.
void Escalonador::ESCALONA(int n) {
    Host *atual;
    int tam = 0;

    atual = hosts.frente;
    tam = atual->urls.GetTamanho();

    if (tam < n) {
        while (n) {
            if (tam <= n) {
                atual->urls.Imprime(this->nome_arq_saida);
                atual->urls.Limpa();
                n -= tam;
                atual = atual->prox;
                tam = atual->urls.GetTamanho();
            }
            else {
                atual->urls.Imprime(this->nome_arq_saida, n);
                atual->urls.Limpa(n);
                n = 0;
            }
            
        }
        
    }
    else if (tam > n) {
        atual->urls.Imprime(this->nome_arq_saida, n);
        atual->urls.Limpa(n);
    }
    else {
        atual->urls.Imprime(this->nome_arq_saida);
        atual->urls.Limpa();
    }
}





//  ESCALONA_HOST <host> <quantidade>: são escalonadas apenas uma quantidade especificada de URLs somente deste host.
void Escalonador::ESCALONA_HOST(string host, int n) {
    Host *atual;
    atual = hosts.frente;
    while (atual != NULL && atual->conteudo != host) 
        atual = atual->prox;
    if (atual != NULL) {
        atual->urls.Imprime(this->nome_arq_saida, n);
        atual->urls.Limpa(n);
    }
}





//  VER_HOST <host>: exibe todas as URLs do host, na ordem de prioridade(numero de barras determina a prioridade).
void Escalonador::VER_HOST(string host) {
    Host *atual;

    atual = hosts.frente;

    while (atual != NULL && atual->conteudo != host) 
        atual = atual->prox;

    if (atual != NULL) {
        atual->urls.Imprime(this->nome_arq_saida);
    }
}





//  LISTA_HOSTS: exibe todos os hosts, seguindo a ordem em que foram conhecidos.
void Escalonador::LISTA_HOSTS() {
    Host *atual;

    ofstream arquivo_saida;
    arquivo_saida.open(this->nome_arq_saida, ios::app);
    if (! arquivo_saida) {
        cout << "Arquivo de saida nao pode ser aberto" << endl;
        abort();
    }

    atual = hosts.frente->prox;

    if (atual != NULL && ArquivoVazio(this->nome_arq_saida)) {
        arquivo_saida << atual->conteudo;
        atual = atual->prox;
        while (atual != NULL) {
            arquivo_saida << endl << atual->conteudo;
            atual = atual->prox;
        }
    }
    else {
        while (atual != NULL) {
            arquivo_saida << endl << atual->conteudo;
            atual = atual->prox;
        }
    }

    
    arquivo_saida.close();
}





//  LIMPA_HOST <host>: limpa a lista de URLs do host.
void Escalonador::LIMPA_HOST(string host) {
    Host *atual;

    atual = hosts.frente;

    while (atual->conteudo != host && atual != NULL) 
        atual = atual->prox;

    if (atual != NULL) {
        atual->urls.Limpa();
    }
}





//  LIMPA_TUDO: limpa todas as URLs, inclusive os hosts.
void Escalonador::LIMPA_TUDO() {
    Host *atual;
    atual = hosts.frente;

    for (int i = 0; i < hosts.tamanho; i++) {
        atual->urls.Limpa();
        atual = atual->prox;
    }

    hosts.Limpa();
}




// Grava o nome do arquivo de saida
void Escalonador::NomeArqSaida(string nome) {
    this->nome_arq_saida = nome;
}