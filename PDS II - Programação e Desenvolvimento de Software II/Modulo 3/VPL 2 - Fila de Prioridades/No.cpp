#include "No.hpp"

No::No(int prioridade, int dado, No* proximo) {
    this->_prioridade = prioridade;
    this->_dado = dado;
    this->_proximo = proximo;
}

void No::setProximo(No *next) {
    this->_proximo = next;
}

No* No::getProximo() {
    return this->_proximo;
}

void No::setDado(int dado) {
    this->_dado = dado;
}

int No::getDado() {
    return this->_dado;
}

void No::setPrioridade(int prioridade) {
    this->_prioridade = prioridade;
}

int No::getPrioridade() {
    return this->_prioridade;
}