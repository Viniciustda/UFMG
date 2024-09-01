
#include "primeira_etapa.hpp"
#include "segunda_etapa.hpp"
#include "arq_funcao.hpp"
#include <iostream>
#include <cstdio>
#include <string>


using namespace std;

int main(int argc, char const *argv[]) {

    // Parametros que serao passados
    string nome_arq_entrada;
    string nome_arq_saida;
    int num_ent_rod = 0;
    int num_fitas = 0;

    // Recebe os parametros
    nome_arq_entrada = argv[1];
    nome_arq_saida = argv[2];
    num_ent_rod = stoi(argv[3]);
    if (argv[4] != NULL)                // Parametro opcional - usado para limitar o numero de fitas. Assim eh possivel decidir quantas entidades serao lidas no total
        num_fitas = stoi(argv[4]);

    // Variaveis
    int num_rodada = 1;
    int total_entidades = 0; 
    bool existe_limite = false;

    // Verifica se existe um limite de fitas
    if (num_fitas > 0) 
        existe_limite = true;

    // Conta quantas entidades tem no arquivo
    total_entidades = ContaLinhas(nome_arq_entrada);

    // Caso seja definido o numero de fitas, mas nao o numero de entidades para cada fita, as entiadades serao distribuidas entre essas fitas
    if (existe_limite && !num_ent_rod) 
        num_ent_rod = float(total_entidades) / float(num_fitas) + 0.5;
    else if (existe_limite && total_entidades > num_fitas * num_ent_rod)
        total_entidades = num_fitas * num_ent_rod;
        

    // Executa a primeira etapa do TP
    PrimeiraEtapa(nome_arq_entrada, num_ent_rod, num_fitas, existe_limite, &num_rodada);

    // Executa a segunda etapa do TP
    SegundaEtapa(nome_arq_saida, num_rodada, total_entidades);


    return 0;
}