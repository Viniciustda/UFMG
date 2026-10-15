#include <stdio.h>

void soma1(int * x){
    (*x)++;
}

void main(){
    int v = 12;
    int *t;
    t = &v;
    printf("%d",soma1(*t));
}