#include <math.h>
#include "estatistica.h"

#define pi 3.141592

float cauchy(float x) {
	return 1 / (pi * (1 + pow(x, 2))) ;
}

float gumbel(float x, float mi, float beta) {
	float z = (x - mi) / beta;
	return (1 / beta) * exp(-(z + exp(-z)));
}

float laplace(float x, float mi, float b) {
	return (1 / (2 * b)) * exp(-fabs(x - mi) / b);
}