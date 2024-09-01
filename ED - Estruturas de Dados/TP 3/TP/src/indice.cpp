#include "indice.hpp"
#include "termo.hpp"
#include "hashing.hpp"
#include "filaencadeada.hpp"
#include "arq_funcao.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <dirent.h>
#include <cstdio>

using namespace std;

void CriaIndice(string arq_consulta, string arq_ranking, string pasta_corpus, string arq_stopwords, int num_arq, int tam_vocabulario, Termo *indice) {


    // ---------------------------------Descobre o nome dos arquivos do corpus---------------------------------

    DIR *dir;
    struct dirent *dir_aux;
    FilaEncadeada nome_arquivos;

    dir = opendir(pasta_corpus.c_str());

    dir_aux = readdir(dir);    // Pula os diretorios da pasta
    dir_aux = readdir(dir);

    while ( ( dir_aux = readdir(dir) ) != NULL ) nome_arquivos.Enfileira(dir_aux->d_name);

    closedir(dir);

    // ---------------------------------Para cada arquivo no corpus---------------------------------
    // Preenche o indice

    string nome_arq_corpus;
    string copia;
    string linha;
    string palavra;
    string id_arq;
    int arq_atual = 0;
    int hash = 0;

    while (arq_atual < num_arq ) {
        nome_arq_corpus = nome_arquivos.Desenfileira();
    
        // Define o id do arquivo
        id_arq = nome_arq_corpus;
        for (int i = 0; i < 4; i++) {
            id_arq.pop_back();
        }
        
        nome_arq_corpus = pasta_corpus + "/" + nome_arq_corpus;

        // Abre o arquivo atual

        ifstream arquivo_entrada;
        arquivo_entrada.open(nome_arq_corpus);
        if (! arquivo_entrada) {
            cout << "Arquivo de entrada (10) nao pode ser aberto" << endl;
            abort();
        }

        while (!arquivo_entrada.eof()) {

            getline(arquivo_entrada, linha);

            if (linha != "") {
                char * clinha = new char [linha.length()+1];
                strcpy (clinha, linha.c_str());

                // Para cada palavra do arquivo
                char * p = strtok (clinha," ");
                while (p!=0) {
                    palavra = p;

                    // Calcula o hash da palavra
                    hash = Hash(palavra, tam_vocabulario);

                    // Se a posicao do hash da palavra esta vazia, adiciona a palavra no indice nessa posicao
                    if (indice[hash].GetTermo() == "") {
                        indice[hash].SetTermo(palavra);
                        indice[hash].frequecia.Insere(stoi(id_arq));
                        indice[hash].IncrementaFrequencia(stoi(id_arq));
                    } 
                    // Se essa palavra ja ta nessa posicao, aumenta a frequencia dela
                    else if (indice[hash].GetTermo() == palavra) {
                        if (!indice[hash].frequecia.PesquisaID(stoi(id_arq))) {
                            indice[hash].frequecia.Insere(stoi(id_arq));
                        }
                        indice[hash].IncrementaFrequencia(stoi(id_arq));
                    }
                    // Se a posicao esta ocupada por outra palavra, vai aumentando o hash em um e procurando se a palavra ja esta no indice ou se sera necessario adicionar na proxima posicao vazia
                    else {
                        while (1) {
                            hash++;
                            if (hash == tam_vocabulario) hash = 0;
                            if (indice[hash].GetTermo() == palavra) {
                                if (!indice[hash].frequecia.PesquisaID(stoi(id_arq))) {
                                    indice[hash].frequecia.Insere(stoi(id_arq));
                                }
                                indice[hash].IncrementaFrequencia(stoi(id_arq));
                                break;
                            }
                            else if (indice[hash].GetTermo() == "") {
                                indice[hash].SetTermo(palavra);
                                indice[hash].frequecia.Insere(stoi(id_arq));
                                indice[hash].IncrementaFrequencia(stoi(id_arq));
                                break;
                            }
                            
                        }
                        
                    }

                    p = strtok(NULL," ");
                }
                    
                delete[] clinha;
            }
        }
        
        // Fecha o arquivo atual
        arquivo_entrada.close();

        // Passa para o proximo arquivo
        arq_atual++;
    }
       
}