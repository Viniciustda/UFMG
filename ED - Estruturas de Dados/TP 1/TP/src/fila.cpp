#include "host.hpp"
#include "fila.hpp"
#include <iostream>
#include <string>

using namespace std;

FilaEncadeada::FilaEncadeada() : Fila() {
    frente = new Host; // Célula cabeça;
    tras = frente;
}

FilaEncadeada::~FilaEncadeada() {
    Limpa();
    delete frente;
}

// Adiciona host na fila
void FilaEncadeada::Enfileira(string conteudo) {
    Host *nova;
    nova = new Host();
    nova->conteudo = conteudo;
    tras->prox = nova;
    tras = nova;
    tamanho++;
}

// Remove host da fila
string FilaEncadeada::Desenfileira() {
    Host *p;
    string aux;
    if (tamanho == 0)
        throw "Fila está vazia!";
    aux = frente->prox->conteudo;
    p = frente;
    frente = frente->prox;
    delete p;
    tamanho--;
    return aux;
}

// Limpa fila
void FilaEncadeada::Limpa() {
    Host *p;
    p = frente->prox;
    while(p!=NULL){
        frente->prox = p->prox;
        delete p;
        p = frente->prox;
    }
    tamanho = 0;
    tras = frente;
}

// Confere se determinado host existe
bool FilaEncadeada::ConfereHost(string procurado) {
    bool existe = false;
    Host *p;
    if (tamanho == 0)
        return existe;
    p = frente->prox;
    
    while (p!=NULL) {
        if (p->conteudo == procurado) {
            existe = true;
            break;
        }
        p = p->prox;
    }
    return existe;
}