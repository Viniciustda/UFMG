#include "frequencia.hpp"
#include "arq_funcao.hpp"
#include "lista.hpp"
#include <iostream>
#include <string>

using namespace std;

ListaEncadeada::ListaEncadeada() : Lista() {
    primeiro = new Frequencia();
    ultimo = primeiro;
}

ListaEncadeada::~ListaEncadeada() {
    Limpa();
    delete primeiro;
}

// Posiciona a Frequencia de acordo com o numero de barras
Frequencia* ListaEncadeada::Posiciona(int id, bool antes=false) {

    Frequencia *p; 

    // Posiciona na célula anterior a desejada
    p = primeiro;

    while (p->prox != NULL && p->prox->ID <= id) 
        p = p->prox;

    // vai para a próxima
    // se antes for false
    if(!antes)
        p = p->prox;

    return p;
}

// Retorna o conteudo da Frequencia. O conteudo eh o link
int ListaEncadeada::GetFrequencia(int id) {
    Frequencia *p;
    p = Posiciona(id, true);
    return p->freq;
}

// Retorna o ID do primeiro arquivo
int ListaEncadeada::GetIDPrimeiro() {
    return primeiro->prox->ID;
}

// Retorna o ID do proximo arquivo
int ListaEncadeada::GetProxID(int id) {
    Frequencia *p;
    p = primeiro->prox;
    
    while (p!=NULL) {
        if (p->ID == id) {
            break;
        }
        p = p->prox;
    }
    return p->prox->ID;
}

// Define o conteudo da Frequencia. O conteudo eh o link
void ListaEncadeada::IncrementaFrequencia(int id) {
    Frequencia *p;
    p = Posiciona(id, true);
    p->freq++;
    
}

// Insere uma Frequencia no final da lista
void ListaEncadeada::Insere(int id) {
    Frequencia *nova;
    nova = new Frequencia();
    nova->ID = id;
    ultimo->prox = nova;
    ultimo = nova;
    tamanho++;
}

// Limpa todas as Frequencias
void ListaEncadeada::Limpa() {
    Frequencia *p;
    p = primeiro->prox;
    while (p!=NULL) {
        primeiro->prox = p->prox;
        delete p;
        p = primeiro->prox;
    }
    ultimo = primeiro;
    tamanho = 0;
}

// Confere se existe determinada Frequencia
bool ListaEncadeada::PesquisaID(int id_procurado) {
    bool existe = false;
    Frequencia *p;
    if (tamanho == 0)
        return existe;
    p = primeiro->prox;
    
    while (p!=NULL) {
        if (p->ID == id_procurado) {
            existe = true;
            break;
        }
        p = p->prox;
    }
    return existe;
}