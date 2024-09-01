#include "url.hpp"
#include <iostream>
#include <string>

using namespace std;

Url::Url() {
    this->num_barras = 0;
    this->conteudo = "sem_conteudo";
    this->prox = NULL; 
}