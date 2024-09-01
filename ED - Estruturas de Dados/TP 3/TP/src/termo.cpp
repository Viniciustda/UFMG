#include "termo.hpp"
#include <iostream>
#include <string>

using namespace std;

Termo::Termo() {
    this->palavra = "";
}

Termo::~Termo() {}

string Termo::GetTermo() {
    return this->palavra;
}

void Termo::SetTermo(string palavra) {
    this->palavra = palavra;
}

int Termo::GetFrequencia(int id) {
    return this->frequecia.GetFrequencia(id);
}

int Termo::GetIDPrimeiro() {
    return this->frequecia.GetIDPrimeiro();
}

int Termo::GetProxID(int id) {
    return this->frequecia.GetProxID(id);
}

void Termo::IncrementaFrequencia(int id) {
    this->frequecia.IncrementaFrequencia(id);
}