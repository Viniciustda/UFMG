#include <stdio.h>
#include <vector>
#include <iostream>
#include <sstream>

#include "Ponto.hpp"
#include "Ponto2D.hpp"
#include "Ponto3D.hpp"
#include "Ponto4D.hpp"

Ponto* lerProximoPonto(std::string line) {
  std::istringstream ss(line);
  std::string token;
  std::getline(ss, token, ' ');
  double x = stod(token);
  if (std::getline(ss, token, ' ')) {
    double y = stod(token);
    if (std::getline(ss, token, ' ')) {
      double z = stod(token);
      if (std::getline(ss, token, ' ')) {
		double w = stod(token);
		return new Ponto4D(x, y, z, w);
	  }
      return new Ponto3D(x, y, z);
    } else {
      return new Ponto2D(x, y);
    }
  }
}

int main() {
  // le o arquivo
  std::vector<Ponto*> pontos;
  std::string line;
  while (std::getline(std::cin, line)) {
    // Converte a linha em um ponto, e o insere no vetor:
    pontos.push_back(lerProximoPonto(line));
  }
  // Soma das normas:
  double norm = 0.0;
  
  
  for (Ponto* v1: pontos) {
	for (Ponto* v2: pontos) {
		if(v1->getDimensao() == v2->getDimensao() && v1 != v2){
			printf("%.2lf ", v1->distancia(v2));
		}
	}
	printf("\n");
  }
  
  for (Ponto* v1: pontos) {
		if(v1->getDimensao() == 2){
		    Ponto2D* p2d = dynamic_cast<Ponto2D*>(v1);
		    p2d->converteParaPolar();
		    std::cout << p2d->to_string() << "\n";
		}
	}
  return 0;
}
