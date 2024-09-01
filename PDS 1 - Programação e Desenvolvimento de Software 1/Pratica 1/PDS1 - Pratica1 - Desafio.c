#include <stdio.h>

void main() {

	int cpf, cpfaux, resto, peso, soma, dezena, unidade;
	printf("Digite seu cpf (sem o digito verificador): ");
	scanf("%d", &cpf);
	cpfaux = cpf;
	soma = 0;
	peso = 2;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	if(soma % 11 == 0 || soma % 11 ==1)
	{
		dezena = 0;
	}
	else
	{
		dezena = 11 - (soma % 11);
	}
	cpfaux = cpf;
	soma = 2 * dezena;
	peso = 3;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	cpfaux /= 10;
	peso ++;
	resto = cpfaux % 10;
	soma += resto * peso;
	if(soma % 11 == 0 || soma % 11 ==1)
	{
		unidade = 0;
	}
	else
	{
		unidade = 11 - (soma % 11);
	}
	printf("Seu cpf com o numero verificador eh: %d-%d%d", cpf, dezena, unidade);
	
}