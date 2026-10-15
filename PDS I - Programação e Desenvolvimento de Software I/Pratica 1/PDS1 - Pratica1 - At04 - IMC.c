#include <stdio.h>

void main() {

	float ab, pb, imcb, pib, ao, po, imco, pio;
	
	ab = 1.84;           //altura Brutus
	pb = 122;            //peso Brutus
	imcb = pb/(ab*ab);   //IMC Brutus
	pib = 24.99*ab*ab;      //peso ideal Brutus

	ao = 1.76;           //altura Olivia
	po = 45;             //peso Olivia
	imco = po/(ao*ao);   //IMC Olivia
	pio = 18.5*ao*ao;    //peso ideal Olivia


	// Brutus

	printf("\n O IMC de Brutus eh: %.2f", imcb);
	printf("\n Brutus precisa perder %.2fkg para atingir o peso saudavel\n",pb-pib);


	// Olivia

	printf("\n O IMC de Olivia eh: %.2f", imco);
	printf("\n Olivia precisa ganhar %.2fkg para atingir o peso saudavel",pio-po);


}