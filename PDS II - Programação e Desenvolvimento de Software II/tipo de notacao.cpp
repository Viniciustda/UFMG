/* Este programa utiliza Aritm�tica de Apontadores e demonstra as diferentes notac�es 
   para referenciar elementos de um array */

#include <stdio.h>
#include <stdlib.h>

#define TAM 5

void imprime_array(int[], int);

int main() {
    int i, array[5] = {10, 20, 30, 40, 50}; /* o identificador array � um APONTADOR !!! */ 
    int *pont;
    
    printf("Este programa utiliza Aritmetica de Apontadores e demonstra as diferentes "
           "notacoes para referenciar elementos de um array:\n\n");
    
    printf("Elementos do array: ");    
    imprime_array(array, TAM);
       
    printf("\n\nOs elementos de um array podem ser referenciados com NOTACAO DE SUBSCRITO utilizando "
           "o proprio nome do array ou de outro ponteiro que aponte para o array\n(int *pont; pont = array;)\n\n");
    printf("Notacao de Subscrito    Notacao de Subscrito \n");
    printf("Utilizando Array        Utilizando Outro Ponteiro\n\n");    
    
    pont = array;
    for (i = 0; i <= TAM - 1; i++) {
        printf("array [%d] = %d", i, array[i]);     /*IMPRIMEM O MESMO CONTE�DO */  
        printf("\t\tpont [%d] = %d\n", i, pont[i]);  
    }      

    printf("\n\nOs elementos de um array podem ser referenciados com NOTACAO PONTEIRO/DESLOCAMENTO. Sendo o "
           "ponteiro o proprio nome do array ou de outro ponteiro\n(int *pont; pont = array;)\n");
    printf("\nNotacao                 Notacao                 Notacao ");
    printf("\nArray                   Array/Deslocamento      Outro Ponteiro/Deslocamento\n\n");    
    

    for (i = 0; i <= TAM - 1; i++) {
        printf("array [%d] = %d  ", i, array[i]);
        printf("\t*(array + %d) = %d", i, *(array + i)); /*IMPRIMEM O MESMO CONTE�DO */ 
        printf("\t*(pont + %d) = %d\n", i, *(pont + i));  
    }  
                                                        
    printf("\n\n");   
    return 0;
}

void imprime_array(int array[], const int tamanho) {
    int indice;
    
    for(indice = 0; indice <= tamanho - 1; indice++)
        printf("%d  ",array[indice]);
    return;
}
