#include "segunda_etapa.hpp"
#include "entidade.hpp"
#include "heap.hpp"
#include "arq_funcao.hpp"
#include <iostream>
#include <fstream>
#include <string>


using namespace std;

void SegundaEtapa(string nome_arq_saida, int num_rodada, int total_entidades) {

    // Variaveis
    Entidade heap[num_rodada];
    fstream fita;
    string nome_fita;
    string entrada;
    int num_rodada_aux = num_rodada-1;
    int indice = 0;
    
    // Abre o arquivo de saida
    ofstream arquivo_saida;
    arquivo_saida.open(nome_arq_saida);
    if (! arquivo_saida) {
        cout << "Arquivo de saida nao pode ser aberto (1)" << endl;
        abort();
    }

    // Executa este procedimento uma vez para cada fita (num_rodada)
    
    while (num_rodada > 0) {

        // Gera o nome do arquivo (fita) que sera aberto
        nome_fita = "rodada-" + to_string(num_rodada) + ".txt";

        // Abre a fita para ler as entidades
        fita.open(nome_fita);
        if (! fita) {
            cout << "A fita " << num_rodada << " (2) nao pode ser aberta" << endl;
            abort();
        }

        // Insere a primeira entidade da fita no heap
        getline(fita, entrada);
        if (entrada != "") {
            heap[indice].Preenche(entrada, num_rodada);
            indice++;
        }

        // Fecha a fita
        fita.close();

        // Apaga esta entidade da fita
        arquivo_saida.close();
        ApagaPrimeiraLinhaArquivo(nome_fita);
        arquivo_saida.open(nome_arq_saida, ios::app);
        if (! arquivo_saida) {
            cout << "Arquivo de saida nao pode ser aberto (3)" << endl;
            abort();
        }

        num_rodada--;
    }
   
    // Ordena o heap
    Heapsort(heap, num_rodada_aux);

    // O trecho roda ate que todas as entidades sejam passadas para o arquivo de saida
    indice = 0;
    while (total_entidades--) {      

        // Imprime a primeira entidade do heap no arquivo de saida
        arquivo_saida << heap[0].GetConteudo() << endl;

        // Gera o nome do arquivo (fita) que sera aberto
        nome_fita = "rodada-" + to_string(heap[0].GetRodada()) + ".txt";
        
        // Abre a fita para ler as entidades
        fita.open(nome_fita);
        if (! fita) {
            cout << "A fita " << heap[0].GetRodada() << " (3) nao pode ser aberta" << endl;
            abort();
        }

        // Se a fita nao esta vazia
        if (!ArquivoVazio(nome_fita)) {
            // Insere a primeira entidade da fita no heap
            getline(fita, entrada);
            if (entrada != "")
                heap[0].Preenche(entrada, heap[0].GetRodada());

            // Ordena o heap
            Heapsort(heap, num_rodada_aux);

            // Apaga esta entidade da fita
            ApagaPrimeiraLinhaArquivo(nome_fita);

        }
        else {

            // "Exclui" a celula
            heap[0] = heap[num_rodada_aux];
            num_rodada_aux--;
            
            // Ordena o heap
            Heapsort(heap, num_rodada_aux);  
        }

        // Fecha a fita
        fita.close();
    }

    // Fecha o arquivo de saida
    arquivo_saida.close();

}