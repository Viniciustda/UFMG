#include "url.hpp"
#include "funcao_arq.hpp"
#include "lista.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

ListaEncadeada::ListaEncadeada() : Lista() {
    primeiro = new Url();
    ultimo = primeiro;
}

ListaEncadeada::~ListaEncadeada() {
    Limpa();
    delete primeiro;
}

// Posiciona a url de acordo com o numero de barras
Url* ListaEncadeada::Posiciona(int n_barras, bool antes=false) {

    Url *p; 

    // Posiciona na célula anterior a desejada
    p = primeiro;

    while (p->prox != NULL && p->prox->num_barras <= n_barras) 
        p = p->prox;

    // vai para a próxima
    // se antes for false
    if(!antes)
        p = p->prox;

    return p;
}

// Retorna o conteudo da url. O conteudo eh o link
string ListaEncadeada::GetConteudo(int pos) {
    Url *p;
    p = Posiciona(pos);
    return p->conteudo;
}

// Define o conteudo da url. O conteudo eh o link
void ListaEncadeada::SetConteudo(string conteudo, int pos) {
    Url *p;
    p = Posiciona(pos);
    p->conteudo = conteudo;
}

// Insere uma url no inicio da lista
void ListaEncadeada::InsereInicio(string conteudo, int n_barras) {
    Url *nova;
    nova = new Url();
    nova->conteudo = conteudo;
    nova->prox = primeiro->prox;
    nova->num_barras = n_barras;
    primeiro->prox = nova;
    tamanho++;
    if(nova->prox == NULL)
        ultimo = nova;
}

// Insere uma url no final da lista
void ListaEncadeada::InsereFinal(string conteudo, int n_barras) {
    Url *nova;
    nova = new Url();
    nova->conteudo = conteudo;
    nova->num_barras = n_barras;
    ultimo->prox = nova;
    ultimo = nova;
    tamanho++;
}

// Insere uma url ja na posicao correta de acordo com o numero de barras
void ListaEncadeada::InserePosicao(string conteudo, int n_barras) {

    Url *p, *nova;

    p = Posiciona(n_barras,true); // posiciona na célula anterior

    nova = new Url();
    nova->conteudo = conteudo;
    nova->num_barras = n_barras;
    nova->prox = p->prox;
    p->prox = nova;
    tamanho++;

    if(nova->prox == NULL)
        ultimo = nova;
}

// Remove a primeira url da lista
string ListaEncadeada::RemoveInicio() {
    string aux;
    Url *p;
    if (tamanho == 0)
        throw "ERRO: Lista vazia!";
    p = primeiro->prox;
    primeiro->prox = p->prox;
    tamanho--;
    if(primeiro->prox == NULL)
        ultimo = primeiro;
    aux = p->conteudo;
    delete p;
    return aux;
}

// Remove a ultima url da lista
string ListaEncadeada::RemoveFinal() {
    string aux;
    Url *p;
    if (tamanho == 0)
        throw "ERRO: Lista vazia!";
    // posiciona p na celula anterior à última
    p = Posiciona(tamanho, true);
    p->prox = NULL;
    tamanho--;
    aux = ultimo->conteudo;
    delete ultimo;
    ultimo = p;
    return aux;
}

// Remove uma url especifica
string ListaEncadeada::RemovePosicao(int pos) {
    string aux;
    Url *p, *q;
    if (tamanho == 0)
        throw "ERRO: Lista vazia!";
    // posiciona p na celula anterior à pos
    p = Posiciona(pos, true);
    q = p->prox;
    p->prox = q->prox;
    tamanho--;
    aux = q->conteudo;
    delete q;
    if(p->prox == NULL)
        ultimo = p;
    return aux;
}

// Imprime todas as urls
void ListaEncadeada::Imprime(string nome_arq_saida) {
    Url *p;

    ofstream arquivo_saida;
    arquivo_saida.open(nome_arq_saida, ios::app);
    if (! arquivo_saida) {
        cout << "Arquivo de saida nao pode ser aberto" << endl;
        abort();
    }
    p = primeiro->prox;
    if (p != NULL && ArquivoVazio(nome_arq_saida)) {
        arquivo_saida << p->conteudo;
        p = p->prox;
        while (p!=NULL) {
            arquivo_saida << endl << p->conteudo;
            p = p->prox;
        }
    }
    else {
        while (p!=NULL) {
            arquivo_saida << endl << p->conteudo;
            p = p->prox;
        }
    }
    arquivo_saida.close();
}

// Imprime determinada quantidade de urls
void ListaEncadeada::Imprime(string nome_arq_saida, int n) {
    Url *p;

    ofstream arquivo_saida;
    arquivo_saida.open(nome_arq_saida, ios::app);

    if (! arquivo_saida) {
        cout << "Arquivo de saida nao pode ser aberto" << endl;
        abort();
    }

    p = primeiro->prox;
    
    if (p != NULL && ArquivoVazio(nome_arq_saida)) {
        
        arquivo_saida << p->conteudo;
        p = p->prox;
        for (int i = 0; i < n-1; i++) {
            arquivo_saida << endl << p->conteudo;
            p = p->prox;
        }
    }
    else {
        for (int i = 0; i < n; i++) {
            arquivo_saida << endl << p->conteudo;
            p = p->prox;
        }
    }

    arquivo_saida.close();
}

// Limpa todas as urls
void ListaEncadeada::Limpa() {
    Url *p;
    p = primeiro->prox;
    while (p!=NULL) {
        primeiro->prox = p->prox;
        delete p;
        p = primeiro->prox;
    }
    ultimo = primeiro;
    tamanho = 0;
}

// Limpa determinada quantidade de urls
void ListaEncadeada::Limpa(int n) {
    Url *p;
    int i = 0;
    p = primeiro->prox;
    for (i = 0; i < n; i++) {
        primeiro->prox = p->prox;
        delete p;
        p = primeiro->prox;
    }
    
    tamanho -= n;
    if (tamanho == 0)   
        ultimo = primeiro;
    
}

// Confere se existe determinada url
bool ListaEncadeada::ConfereUrl(string procurado) {
    bool existe = false;
    Url *p;
    if (tamanho == 0)
        return existe;
    p = primeiro->prox;
    
    while (p!=NULL) {
        if (p->conteudo == procurado) {
            existe = true;
            break;
        }
        p = p->prox;
    }
    return existe;
}