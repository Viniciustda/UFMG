typedef struct Stats {
	float a_idade_media;
	float b_maior_IMC;
	char c_nome_MVP[1000];	
} Stats;

void imprimeMatrizDiag(int **M_diag, int n);

//------------A1------------
int fatorial(int n);

float euler(int n);

//------------A2------------
int potenciaDe2(int n);

float serieDoisTercos(int n);

//------------A3------------
int caixaAlta(char *str);

//------------A4------------
int** matrizDiag(int **M, int n); 

//------------A5------------
int saoAmigos(int **M, int i, int j); 

//------------A6------------
Stats estatisticasNBA();