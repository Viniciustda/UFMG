#include <stdio.h>
#include "funcoes8.1.h"

/*------------------------------------------------------------------------------------------------*/

void imprime_matriz(int M[][TAM_MAX], int m, int n) {
    printf("\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", M[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/*------------------------------------------------------------------------------------------------*/

void translada_zeros(int M[][TAM_MAX], int m, int n)
{
    int aux;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (M[i][j] == 0)
            {
                for (int x = i-1; x >= 0; x--)
                {
                    aux = M[x][j];
                    if (aux == 0)
                        break;
                    else
                    {
                        M[x][j] = M[x+1][j];
                        M[x+1][j] = aux;
                    }
                }
            }
        }
    }
}

/*------------------------------------------------------------------------------------------------*/

int zera_3consec(int M[][TAM_MAX], int m, int n)
{
    int M_linha[TAM_MAX][TAM_MAX], M_coluna[TAM_MAX][TAM_MAX];
    int consec, anterior;
    int zeros = 0;

    for (int i = 0; i < m; i++) 
    {
        consec = 0;
        anterior = -1;
        for (int j = 0; j < n; j++) 
        {
            M_linha[i][j] = M[i][j];
            if (j != n - 1) 
            {
                if (M_linha[i][j] != anterior) 
                {
                    if (consec >= 3) 
                        for (int x = consec; x > 0; x--) 
                            M_linha[i][j-x] = 0;
                    consec = 1;
                } 
                else 
                    consec++;
            } 
            else 
            {
                if (M_linha[i][j] != anterior) 
                {
                    if (consec >= 3) 
                    {
                        for (int x = consec; x > 0; x--) 
                        {
                            M_linha[i][j-x] = 0;
                        }
                    }
                } 
                else if(consec >= 2) 
                    for (int x = consec; x >= 0; x--) 
                            M_linha[i][j-x] = 0;
            }
            
            

            anterior = M[i][j];
        }
    }

    for (int j = 0; j < n; j++) 
    {
        consec = 0;
        anterior = -1;
        for (int i = 0; i < m; i++) 
        {
            M_coluna[i][j] = M[i][j];
            if (i != m - 1) 
            {
                if (M_coluna[i][j] != anterior) 
                {
                    if (consec >= 3) 
                        for (int x = consec; x > 0; x--) 
                            M_coluna[i-x][j] = 0;
                    consec = 1;
                } 
                else 
                    consec++;
            } 
            else 
            {
                if (M_coluna[i][j] != anterior) 
                {
                    if (consec >= 3) 
                        for (int x = consec; x > 0; x--) 
                            M_coluna[i-x][j] = 0;
                } 
                else if(consec >= 2) 
                    for (int x = consec; x >= 0; x--) 
                            M_coluna[i-x][j] = 0;
            }
            
            

            anterior = M[i][j];
        }
    }
    
    /*
    printf("\n Matriz com linhas zeradas\n");
    imprime_matriz(M_linha, m, n);
    printf("\n Matriz com colunas zeradas\n");
    imprime_matriz(M_coluna, m, n);
    */

   for (int i = 0; i < m; i++)
       for (int j = 0; j < n; j++)
           if (M_linha[i][j] == 0 || M_coluna[i][j] == 0)
           {
                M[i][j] = 0;
                zeros++;
           }

    return zeros;
}

/*------------------------------------------------------------------------------------------------*/