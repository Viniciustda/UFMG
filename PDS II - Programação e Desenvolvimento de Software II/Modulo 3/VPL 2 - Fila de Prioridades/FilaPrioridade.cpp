#include "FilaPrioridade.hpp"
#include "No.hpp"

FilaPrioridade::FilaPrioridade() {
    this->cabeca = NULL;
    this->fim = NULL;
    this->tamanho = 0;
}

FilaPrioridade::~FilaPrioridade() {
    delete novo_no;
}

void FilaPrioridade::inserir(int prioridade, int dado) {
    novo_no = new No(0, 0, NULL);
    this->novo_no->setPrioridade(prioridade);
    this->novo_no->setDado(dado);


    if (this->tamanho == 0) {
        this->cabeca = this->novo_no;
        this->fim = this->novo_no;
    } 
    else if(this->tamanho == 1) {
        this->fim = this->novo_no;
        this->cabeca->setProximo(this->fim);
    } 
    else {
        No* aux = this->fim;
        this->fim = this->novo_no;
        aux->setProximo(this->fim);
    }
    this->tamanho++;
    ordenaFila();
    
}

void FilaPrioridade::ordenaFila() {

    int iteracoes = 0;
    No* cabeca_aux = NULL;
    No* aux = NULL;
    No* indice_i = this->cabeca;
    for ( int i = 0; i < this->tamanho; i++) {
        No* prioritario = indice_i;

        No* indice_j = indice_i->getProximo();
        for (int j = i+1; j < this->tamanho; j++) {
            if (indice_j->getPrioridade() < prioritario->getPrioridade()) {
                prioritario = indice_j;
            }
            aux = indice_j;
            indice_j = aux->getProximo();
        }
        if (indice_i->getPrioridade() > prioritario->getPrioridade()) {
            int aux = indice_i->getDado();
            indice_i->setDado(prioritario->getDado());
            prioritario->setDado(aux);

            int aux_2 = indice_i->getPrioridade();
            indice_i->setPrioridade(prioritario->getPrioridade());
            prioritario->setPrioridade(aux_2);
        }
        aux = indice_i;
        indice_i = aux->getProximo();
    }

}

int FilaPrioridade::remover() {

    if (!estaVazio()) {
        No* aux = this->cabeca;
        this->cabeca = aux->getProximo();
        int dado = aux->getDado();
        delete aux;
        this->tamanho--;
        return dado;
    }
}

int FilaPrioridade::estaVazio() {
    if (this->tamanho == 0) {
        return 1;
    } else {
        return 0;
    }
}

unsigned FilaPrioridade::getTamanho() {
    return this->tamanho;
}

int FilaPrioridade::getMeio() {
    No* meio = this->cabeca;
    int n = 0;
    if (this->tamanho % 2 == 0) {
        n = this->tamanho/2;
    } else {
        n = (this->tamanho - 1) /2;
    }
    
    for (int i = 0; i < n; i++) {
        No* aux = meio;
        meio = aux->getProximo();
    }

    return meio->getDado();
}

int FilaPrioridade::getUltimo() {
    return this->fim->getDado();
}

void FilaPrioridade::furaFila(int dado) {
    No* furao = new No(1, dado, NULL);
    if (this->tamanho != 0) {
        furao->setProximo(this->cabeca);
        this->cabeca = furao;
    }
    else {
        this->cabeca = furao;
        this->fim = furao;
    }
    this->tamanho++;
}

void FilaPrioridade::print() {
    if (estaVazio()) {
        cout << "Fila vazia" << endl;
        return;
    }

    No* ponteiro = this->cabeca;
    while (ponteiro != NULL) {
        cout << ' ' << ponteiro->getDado();
        ponteiro = ponteiro->getProximo();
    }
}