#include <iostream>
#include <string>

#define TAM 50

using namespace std;

class Caractere {
    private:
        int pos_alfa; 
        int n_elemento;
        int pos_ele; 
        
    public:
        Caractere();
        int GetValor();
        void Preenche(char letra, int num_ele, int pos_elemento);
};

Caractere::Caractere() {
    this->pos_alfa = 0;
    this->n_elemento = 0;
    this->pos_ele = 0;
}

int Caractere::GetValor() {
    return this->pos_alfa + this->n_elemento + this->pos_ele;
}

void Caractere::Preenche(char letra, int num_ele, int pos_elemento) {

    this->n_elemento = num_ele;
    this->pos_ele = pos_elemento;

    if (letra == 'A') {
        this->pos_alfa = 0;
    } else if (letra == 'B') {
        this->pos_alfa = 1;
    } else if(letra == 'C') {
        this->pos_alfa = 2;
    } else if(letra == 'D') {
        this->pos_alfa = 3;
    } else if(letra == 'E') {
        this->pos_alfa = 4;
    } else if(letra == 'F') {
        this->pos_alfa = 5;
    } else if(letra == 'G') {
        this->pos_alfa = 6;
    } else if(letra == 'H') {
        this->pos_alfa = 7;
    } else if(letra == 'I') {
        this->pos_alfa = 8;
    } else if(letra == 'J') {
        this->pos_alfa = 9;
    } else if(letra == 'K') {
        this->pos_alfa = 10;
    } else if(letra == 'L') {
        this->pos_alfa = 11;
    } else if(letra == 'M') {
        this->pos_alfa = 12;
    } else if(letra == 'N') {
        this->pos_alfa = 13;
    } else if(letra == 'O') {
        this->pos_alfa = 14;
    } else if(letra == 'P') {
        this->pos_alfa = 15;
    } else if(letra == 'Q') {
        this->pos_alfa = 16;
    } else if(letra == 'R') {
        this->pos_alfa = 17;
    } else if(letra == 'S') {
        this->pos_alfa = 18;
    } else if(letra == 'T') {
        this->pos_alfa = 19;
    } else if(letra == 'U') {
        this->pos_alfa = 20;
    } else if(letra == 'V') {
        this->pos_alfa = 21;
    } else if(letra == 'W') {
        this->pos_alfa = 22;
    } else if(letra == 'X') {
        this->pos_alfa = 23;
    } else if(letra == 'Y') {
        this->pos_alfa = 24;
    } else if(letra == 'Z') {
        this->pos_alfa = 25;
    } 
    
}


int main() {

    int n_testes = 0;
    int n_elementos = 0;
    int indice = 0;
    int hash = 0;

    string entrada;

    Caractere elemeto[TAM];

    getline(cin, entrada);
    n_testes = stoi(entrada);

    while (n_testes--) {
        
        getline(cin, entrada);
        n_elementos = stoi(entrada);

        hash = 0;

        indice = 0;
        while (n_elementos--) {

            getline(cin, entrada);

            for (int i = 0; i < entrada.length(); i++) {
                elemeto[i].Preenche(entrada[i], indice, i);
                hash += elemeto[i].GetValor();
            }
            
            indice++;
                        
        }

        cout << hash << endl;

    }

    return 0;
}

/*

1257

Você terá como uma entrada várias linhas, cada uma com uma string. O valor de cada caracter é computado como segue:

Valor = (Posição no alfabeto) + (Elemento de entrada) + (Posição do elemento)

Todas posições são baseadas em zero. 'A' tem posição 0 no alfabeto, 'B' tem posição 1 no alfabeto, ... O cálculo de hash retornado é a soma de todos os caracteres da entrada. Por exemplo, se a entrada for:
CBA
DDD

então cada caractere deverá ser computado como segue:

2 = 2 + 0 + 0 : 'C' no elemento 0 posição 0
2 = 1 + 0 + 1 : 'B' no elemento 0 posição 1
2 = 0 + 0 + 2 : 'A' no elemento 0 posição 2
4 = 3 + 1 + 0 : 'D' no elemento 1 posição 0
5 = 3 + 1 + 1 : 'D' no elemento 1 posição 1
6 = 3 + 1 + 2 : 'D' no elemento 1 posição 2

O cálculo final de hash será 2+2+2+4+5+6 = 21.

Entrada
A entrada contém vários casos de teste. A primeira linha de entrada contém um inteiro N que indica a quantidade de casos de teste. Cada caso de teste inicia com um inteiro L (1 ≤ L ≤ 100) que indica a quantidade de linhas que vem a seguir. Cada uma destas L linhas contém uma string com até 50 letras maiúsculas ('A' - 'Z').

Saída
Para cada caso de teste imprima o valor de hash que é calculado conforme o exemplo apresentado acima.

*/