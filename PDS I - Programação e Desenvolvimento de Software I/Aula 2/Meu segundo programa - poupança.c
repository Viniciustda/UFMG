#include<stdio.h>

void main() {
	
	float poupanca;

	poupanca = 500;

	// 1 mes comecou:

	poupanca = poupanca + (poupanca * 0.01);

	// 2 mes comecou:

	poupanca = poupanca + 200;

	poupanca = poupanca + (poupanca * 0.01);

	// 3 mes comecou:

	poupanca = poupanca - 50;

	poupanca = poupanca + (poupanca * 0.01);

	// 4 mes comecou:

	printf("\n O valor da poupanca eh: %f", poupanca);

	
}