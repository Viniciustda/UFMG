#include<stdio.h>

void main() {
	
	float poupanca;
	float juros = 1.0056;

	poupanca = 789.54;

	// 1 mes comecou:

	poupanca *= juros;  

	// 2 mes comecou:

	poupanca +=  303.20;

	poupanca *= juros;               

	// 3 mes comecou:

	poupanca -=  58.25;

	poupanca *= juros;                        

	// 4 mes comecou:

	printf("\n O valor da poupanca eh: R$%.2f\n", poupanca);

	
}