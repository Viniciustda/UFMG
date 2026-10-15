#include <stdio.h>
#include "estatistica.h"

void main() {
	printf("\nCauchy: f(-2) = %f\n", cauchy(-2));
	printf("Gumbel: f(0, 0.5, 2) = %f\n", gumbel(0, 0.5, 2));
	printf("Laplace: f(-6, -5, 4) = %f\n", laplace(-6, -5, 4));

}