#ifndef ENTIDADE
#define ENTIDADE

#include <iostream>
#include <string>

using namespace std;

class Entidade {
    public:
        Entidade();
        void Preenche(string entrada, int rodada);
        int GetVisitas();
        int GetRodada();
        string GetConteudo();
    private:
        int visitas;
        int rodada;
        string conteudo;
};

#endif