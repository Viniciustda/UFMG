#include <stdio.h>
#include <math.h>

//-----Atividade 1-----
float paraMetrosPorSegundo(float v) {
    return v / 3.6;
}

//-----Atividade 2-----
float areaCirculo(float raio) {
    float area;
    return area = 3.141592 * raio * raio;
}

//-----Atividade 3-----
int maior3(int n1, int n2, int n3) {
    if(n1 > n2 && n1 > n3) {
        return n1;
    } 
    else if(n2 > n1 && n2 > n3) { 
        return n2; 
    } 
    else if(n3 > n1 && n3 > n2) { 
        return n3; 
    }
}

//-----Atividade 4-----
int ehPar(int n) {
    if(n % 2 == 0) {
        return 1;
    } 
    else { 
        return 0; 
    } 
}

//-----Atividade 5-----
int ehDivisivelPor3ou5(int N) {
    if(N % 3 == 0 || N % 5 == 0) {
        if(N % 3 != 0 || N % 5 != 0) {
            return 1;
        }
    } 
    else { 
        return 0; 
    } 
}

//-----Atividade 6 e 7-----
float pesoIdeal(float h, char sexo) {
    float pi;
    if(sexo == 'M') {
        return pi = (72.7 * h) - 58;
    }
    else {
        return pi = (62.1 * h) - 44.7;
    }
}

//-----Atividade 7-----
float diferencaPeso(float pa, float pi){
    if(pa - pi == 0 ) {
        return printf("\n Voce ja esta com o peso ideal.\n");
    }else if(pa - pi < 0 ) {
        return printf("\n Voce precisa ganhar %0.3f kg para alcancar seu peso ideal.\n", (pa - pi)*-1);
    }else if(pa - pi > 0 ) {
        return printf("\n Voce precisa perder %0.3f kg para alcancar seu peso ideal.\n", pa - pi);
    }
}

//-----Atividade 8-----
int somaImpares(int m) { 
    int soma = 0, k =1; 
    while(k<=m) { 
        soma += k; 
        k += 2; 
    } 
    return soma; 
}

//-----Atividade 9-----
double fatorial(int M) {
    double fatorial = 1;
    int i = 1; 
    if(M == 0) {
        return fatorial; 
    } 
    else {
        do { 
            fatorial *= i; 
            i ++; 
        } while(i<=M);
        return fatorial;
    }
    
}

//-----Atividade 10-----
int EhDivisivelPor3ou5(int j) {
    if(j % 3 == 0 || j % 5 == 0) {
        if(j % 3 != 0 || j % 5 != 0) {
            return 1;
        }
    } else { 
        return 0; 
    } 
}

int somaNumerosDiv3ou5(int b) {
    int Soma = 0;
    int j = 1;
    while(j <= b) {
        if(EhDivisivelPor3ou5(j) == 1) {
            Soma += j;
            j ++;
        } else {
            j ++;
        }
    }

    return Soma;
    
}

//-----Atividade 11-----
float calculaMedia(int x, int y, int z, int operacao) {

    float Media;
    if (operacao == 1) {
        Media = pow((x*y*x),(1.0/3.0));
        
    }else if (operacao == 2) {
        Media = ((x+2)*(y+3)*(z))/6.0;
        
    }else if (operacao == 3) {
        Media = 3/((1.0/x)+(1.0/y)+(1.0/z));
        
    }else if (operacao == 4) {
        Media = (x+y+z)/3.0;
        
    }
    return Media;
}

//-----Atividade 12-----
int numeroDivisores(int g) {
    int div = 1;
    int ndiv = 0;
    do {
        if(g % div == 0) {
            ndiv ++;
            div ++;
        }else {
            div ++;
        }
    } while(div <= g);
    return ndiv;
}

//-----Atividade 13-----
int enesimoFibonacci(int U) {
    int cont = 2;
    int Fibonacci = 0;
    int auxU1 = 1;
    int auxU2 = 0;
    if(U == 1) {
        Fibonacci = 0;
    }else {
        do{
            auxU2 = auxU1 + Fibonacci;         
            Fibonacci = auxU1;                 
            auxU1 = auxU2;             
            cont ++;                            
        }while(cont <= U);
    }
    
    return Fibonacci;
}

//-----Atividade 14-----
int mdc(unsigned int num1, unsigned int num2) {
    int Mdc = 0;
    unsigned int auxmdc = 1;
    do {
        if(num1 % auxmdc == 0 && num2 % auxmdc == 0) {
            Mdc = auxmdc;
            auxmdc ++;
        }else {
            auxmdc ++;
        }
    } while(auxmdc <= num1 && auxmdc <= num2);
    return Mdc;
}

//-----Atividade 15-----
int mmc(unsigned int num3, unsigned int num4) {
    int Mmc;
    if(num3 < num4) {
        Mmc =  num4;
    }else {
        Mmc = num3;
    }
    while(1) {
        if(Mmc % num3 == 0 && Mmc % num4 == 0)
            break;
        Mmc++;
    }
    return Mmc;
}

//-----Atividade 16-----
int MDC(unsigned int num5, unsigned int num6) {
    int MdC = 0;
    unsigned int auxmdc = 1;
    do {
        if(num5 % auxmdc == 0 && num6 % auxmdc == 0) {
            MdC = auxmdc;
            auxmdc ++;
        }else {
            auxmdc ++;
        }
    } while(auxmdc <= num5 && auxmdc <= num6);
    return MdC;
}

int MMC(unsigned int num5, unsigned int num6) {
    int MmC;
    if(num5 < num6) {
        MmC =  num6;
    }else {
        MmC = num5;
    }
    while(1) {
        if(MmC % num5 == 0 && MmC % num6 == 0)
            break;
        MmC++;
    }
    return MmC;
}