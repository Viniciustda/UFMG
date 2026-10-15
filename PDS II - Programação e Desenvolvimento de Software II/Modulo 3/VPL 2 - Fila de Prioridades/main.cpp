#include <iostream>
#include "FilaPrioridade.hpp"

using namespace std;


int main(int argc, char** argv) {
  int prioridade;
  int dado;
 
  FilaPrioridade *fila = new FilaPrioridade();
  while (cin >> prioridade) {
	cin >> dado;
    fila->inserir(prioridade, dado);
    
  }
  
  fila->print();
  std::cout << std::endl;
  std::cout << "s:" << fila->getTamanho() << ", ";
  std::cout << "m:" << fila->getMeio() << ", ";
  std::cout << "u:" << fila->getUltimo() << ", ";
  std::cout << "r:" << fila->remover() << ", ";
  std::cout << "m:" << fila->getMeio() << ", ";
  std::cout << "u:" << fila->getUltimo() << ", ";
  fila->furaFila(7);
  std::cout << "m:" << fila->getMeio() << ", ";
  std::cout << "u:" << fila->getUltimo() << std::endl;
  fila->print();
  std::cout << std::endl;
  return 0;
}