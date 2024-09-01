#include "primeira_etapa.hpp"
#include "entidade.hpp"
#include "quicksort.hpp"
#include <iostream>
#include <fstream>
#include <string>


using namespace std;

void PrimeiraEtapa(string nome_arq_entrada, int num_ent_rod, int num_fitas, bool existe_limite, int *num_rodada) {

    // Variaveis
    Entidade entidade[num_ent_rod];
    fstream fita;
    string nome_fita;
    string entrada;
    int indice = 0;
    int indice_aux = 0;
    bool existe_entrada_com_zero_visitas = false;
    

    // Abre o arquivo de entrada
    // Passado pela linha de comando
    ifstream arquivo_entrada;
    arquivo_entrada.open(nome_arq_entrada);
    if (! arquivo_entrada) {
        cout << "Arquivo de entrada nao pode ser aberto" << endl;
        abort();
    }
    
    // O trecho roda enqunto nao chegar no final do arquivo de entrada
    while (!arquivo_entrada.eof()) {

        // Gera o nome do arquivo (fita) que sera aberto
        nome_fita = "rodada-" + to_string(*num_rodada) + ".txt";

        // Abre a fita para inserir as entidades
        fita.open(nome_fita, ios::app);
        if (! fita) {
            cout << "A fita " << *num_rodada << " (1) nao pode ser aberta" << endl;
            abort();
        }

        // Le n entidades e adiciona em um vetor
        for (indice = 0; indice < num_ent_rod; indice++) {
            getline(arquivo_entrada, entrada);
            if (entrada != "") {
                entidade[indice].Preenche(entrada, *num_rodada);
            }
            else {
                entidade[indice].Preenche("SemConteudo 0", 0);
            }
            if (entidade[indice].GetVisitas() == 0)
                existe_entrada_com_zero_visitas = true;
            if (arquivo_entrada.eof())
                break;
        }
        
        if (indice == num_ent_rod) {

            // Ordena o vetor usando Quicksort
            QuickSort(entidade, num_ent_rod);

            // Insere as entidades ordenadas do vetor na fita
            for (int i = 0; i < num_ent_rod; i++)
                fita << entidade[i].GetConteudo() << endl;
        }
        else { // Caso nao tenha entidades suficientes para preencher a ultima fita

            // Limpa as entidades que nao foram preenchidas por falta de entradas
            for (indice_aux = indice+1; indice_aux < num_ent_rod; indice_aux++) {
                entidade[indice_aux].Preenche("SemConteudo 0", 0);
            }

            // Ordena o vetor usando Quicksort
            QuickSort(entidade, num_ent_rod);

            // Insere as entidades ordenadas do vetor na fita
            if (existe_entrada_com_zero_visitas) {              // Eh preciso diferenciar a entidade com 0 visitas das entidades vazias
                for (int i = 0; i < num_ent_rod; i++) {
                    if (entidade[i].GetConteudo() != "SemConteudo 0")
                        fita << entidade[i].GetConteudo() << endl;
                }
            }
            else {
                for (int i = 0; i <= indice; i++)
                    fita << entidade[i].GetConteudo() << endl;
            }
        }        
        
        // Fecha a fita e ja atualiza o numero da proxima rodada
        fita.close();
        *num_rodada = *num_rodada + 1;

        // Confere se ja atingiu o maximo de fitas, se tiver um maximo
        if (existe_limite)
            num_fitas--;
        if (existe_limite && !num_fitas)
            break;
        
    }

    *num_rodada = *num_rodada - 1;
   
    // Fecha o arquivo de entrada
    arquivo_entrada.close();
}