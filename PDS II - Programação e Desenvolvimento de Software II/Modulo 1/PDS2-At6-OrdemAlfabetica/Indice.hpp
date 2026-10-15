#include <iostream>
#include <iomanip>
#include <string>
#include <list>

using namespace std;

struct Texto {
    list<string> palavra;

    int repeticoes(int tamanho, list<string> Palavra, string word);
    float Percentual(int tamanho, list<string> Palavra, string word);
};

bool pequeno(string palavra);