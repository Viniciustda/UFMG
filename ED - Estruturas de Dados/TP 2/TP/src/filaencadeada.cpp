#include "arq.hpp"
#include "filaencadeada.hpp"
#include <iostream>
#include <string>

using namespace std;

FilaEncadeada::FilaEncadeada() : Fila() {
    frente = new Arquivo; // Célula cabeça;
    tras = frente;
}

FilaEncadeada::~FilaEncadeada() {
    Limpa();
    delete frente;
}

// Adiciona Arquivo na fila
void FilaEncadeada::Enfileira(string conteudo) {
    Arquivo *nova;
    nova = new Arquivo();
    nova->conteudo = conteudo;
    tras->prox = nova;
    tras = nova;
    tamanho++;
}

// Remove Arquivo da fila
string FilaEncadeada::Desenfileira() {
    Arquivo *p;
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
    Arquivo *p;
    p = frente->prox;
    while(p!=NULL){
        frente->prox = p->prox;
        delete p;
        p = frente->prox;
    }
    tamanho = 0;
    tras = frente;
}