#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {

    system("cls");

    int X[10], Y[10], Z[20];
    int contx = 0, conty = 0;

    printf("\n Digite os 10 valores de X com espacos entre eles (Ex.: 1 2 3 ... 10): ");
    for (int i = 0; i < 10; i++)
        scanf("%d", &X[i]);
    
    printf("\n Digite os 10 valores de Y com espacos entre eles (Ex.: 1 2 3 ... 10): ");
    for (int i = 0; i < 10; i++)
        scanf("%d", &Y[i]);

    for (int i = 0; i < 20; i++) {
        if (i % 2 == 0) {
            Z[i] = X[contx];
            contx++;
        } else {
            Z[i] = Y[conty];
            conty++;
        }
    }
    for (int i = 0; i < 20; i++)
    {
        printf("\n Z[%d] = %d\n", i, Z[i]);
    }
    
    

    return 0;
}
