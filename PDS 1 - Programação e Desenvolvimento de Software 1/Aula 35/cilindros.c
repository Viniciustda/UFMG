/*
Crie k predios de forma cilíndrica em uma superfície de 100m x 100m e calcule o volume total ocupado pelos predios. Tanto k, quanto o raio e a altura dos cilindros devem ser gerados aleatoriamente:
- k deve variar entre 1 e 30;
- o raio deve variar entre 1 e 5 metros;
- a altura deve variar entre 1 e 100 metros.
*/

#include <stdio.h>
#include <math.h>
#include "random.h"

#define MAXPREDIOS 30
#define MAXRAIO 5
#define MAXALTURA 100
#define SIZE 20

typedef struct Ponto {
	float x, y;
} Ponto;

typedef struct Circulo {
	Ponto centro;
	float raio;
}Circulo;

typedef struct Cilindro {
	Circulo base;
	float altura;
} Cilindro;

float areaCirculo(Circulo c) {
	return 3.141592*c.raio*c.raio;
}

float distancia(Ponto p1, Ponto p2) {
	return sqrt(pow(p1.x-p2.x, 2) + pow(p1.y-p2.y, 2));
}

int colisaoCirculos(Circulo c1, Circulo c2) {
	if(distancia(c1.centro, c2.centro) < c1.raio + c2.raio)
		return 1;
	return 0;
}

void criaCilindro(Cilindro *c) {
	float altura = randFloat(1, MAXALTURA);
	(*c).altura = altura;
	float raio = randFloat(1, MAXRAIO);
	//(*c).base.raio = raio;
	c->base.raio = raio;
	
	
	
	c->base.raio = raio;
	c->base.centro.x = randFloat(raio, SIZE-raio);
	c->base.centro.y = randFloat(raio, SIZE-raio);
}

int colisaoPredios(Cilindro predio, Cilindro predios[], int n) {
	int i=0;
	for(i=0; i<n; i++) {
		if(colisaoCirculos(predio.base, predios[i].base))
			return 1;
	}
	return 0;
}

float volumeCilindro(Cilindro c) {
	return areaCirculo(c.base) * c.altura;
}

float volumeCilindro2(float raio_base, float altura) {
	return 3.141592 * raio_base * raio_base * altura;
}

void imprimeCilindro(Cilindro c) {
	//float volCilindro = volumeCilindro(c);
	float volCilindro = volumeCilindro2( c.base.raio, c.altura);
	
	printf("\n(%f, %f) %f %f: %f", c.base.centro.x, c.base.centro.y, c.base.raio, c.altura, volCilindro);
	
}

int main() {

	Cilindro predios[MAXPREDIOS];
	int k = randInt(1, MAXPREDIOS);
	int i=0;
	
	for(i=0; i<k; i++) {
		printf("\nCriando cilindro %d:", i);
		criaCilindro(&predios[i]);
		imprimeCilindro(predios[i]);
		//i-= colisaoPredios(predios[i], predios, i);
		if(colisaoPredios(predios[i], predios, i)) {
			i--;
			printf("\nColisao!");
		}
		else {
			printf("\nOK!");
		}
	}
	
	float volumeTotal = 0;
	for(i=0; i<k; i++) {
		volumeTotal += volumeCilindro(predios[i]);
	}
	printf("\nVolume total: %f", volumeTotal);

/*
	Ponto p1, p2;
	Circulo c1, c2;
	scanf("%f %f", &p1.x, &p1.y);
	scanf("%f %f", &p2.x, &p2.y);
	printf("\ndist: %f", distancia(p1, p2));
	scanf("%f %f", &c1.raio, &c2.raio);
	c1.centro = p1;
	c2.centro = p2;
	printf("\narea circ 1: %f", areaCirculo(c1));
	printf("\ncolisao? %d", colisaoCirculos(c1, c2));
*/
	return 0;
}