#include <stdio.h>
#include "Aula 9 - basquete modulo.h"


int main() {

	float h = 12;
	float a = 2;
	float p = 100;

	float ef = eficiencia(h, a, p);

	printf("eficiencia: %f", ef);
	printf("\npontos: %f", pontos(a));


	return 0;
}