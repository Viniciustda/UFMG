#include "entidade.hpp"
#include <iostream>
#include <string>

using namespace std;

Entidade::Entidade() {
    this->visitas = 0;
    this->conteudo = "SemConteudo";
}

void Entidade::Preenche(string entrada, int rodada) {
    int indice = 0;

    this->rodada = rodada;

    this->conteudo = entrada;

    indice = entrada.find_first_of(' ',0);
    entrada.erase(0,indice);
    this->visitas = stoi(entrada);
}

int Entidade::GetVisitas() {
    return this->visitas;
}

int Entidade::GetRodada() {
    return this->rodada;
}

string Entidade::GetConteudo() {
    return this->conteudo;
}