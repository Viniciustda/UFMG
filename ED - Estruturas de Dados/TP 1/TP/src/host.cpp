#include "host.hpp"
#include "lista.hpp"
#include <iostream>
#include <string>

using namespace std;

Host::Host() {
    this->conteudo = "sem_conteudo";
    this->prox = NULL; 
}
Host::~Host() {}