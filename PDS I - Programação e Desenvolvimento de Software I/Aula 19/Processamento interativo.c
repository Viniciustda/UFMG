#include <stdio.h> 

unsigned int somaN(unsigned int N) { 

    unsigned int soma = 0, k =1; 

    while(k<=N) { 
        soma += k; 
        k++; 
    } 

    return soma; 
} 
 

void calculaSoma() { 

    int N; 
    do { 
        printf("\n Digite N:"); 
        scanf("%d", &N); 

    } while(N<=0); 
    printf("\n Soma: %u", somaN(N)); 
} 


void main() {
    int resp;
    do {
        calculaSoma();
        printf("\n De novo? (1=S, 0=N)"); 
        scanf("%d", &resp);
    } while(resp == 1); 
}

/*

OUTRO POSSIVEL PROGRAMA, MAS MAIS COMPLICADO DE LER


#include <stdio.h> 

unsigned int somaN(unsigned int N) { 

    unsigned int soma = 0, k =1; 

    while(k<=N) { 
        soma += k; 
        k++; 
    } 

    return soma; 
} 

 

void main() { 

    int N; 
    Int resp = 0; 

    do { 

        do {
            printf("\n Digite N:");
            scanf("%d", &N);
        } while(N<=0); 

        printf("\n Soma: %u", somaN(N)); 

        printf("\n De novo? (1=S, 0=N)"); 

        scanf("%d", &resp); 

    } while(resp == 1); 

} 
*/ 