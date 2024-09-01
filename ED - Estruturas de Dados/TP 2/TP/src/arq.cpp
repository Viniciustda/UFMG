#include "arq.hpp"
#include <iostream>
#include <string>

using namespace std;

Arquivo::Arquivo() {
    this->conteudo = "sem_conteudo";
    this->prox = NULL; 
}

Arquivo::~Arquivo() {}