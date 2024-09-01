#include "termo.hpp"
#include "consulta.hpp"
#include "filaencadeada.hpp"
#include "similaridade.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>


using namespace std;

void Consulta(string arq_ranking, string arq_consulta, Termo indice[], int num_arq, int tam_vocabulario) {

    // Calcula peso dos termos nos documentos
    Similaridade** tab_peso = new Similaridade *[num_arq];
    for (int i = 0; i < num_arq; i++) {
        tab_peso[i] = new Similaridade[tam_vocabulario];
    }

    int id = 0;

    for (int i = 0; i < tam_vocabulario; i++) {

        id = indice[i].GetIDPrimeiro();
        for (int j = 0; j < indice[i].frequecia.GetTamanho(); j++) {
        
            tab_peso[j][i].conteudo = (float)indice[i].GetFrequencia(id) * ( log ( (float)num_arq / (float)indice->frequecia.GetTamanho() ) );
            tab_peso[j][i].id = id;
            if (j < indice[i].frequecia.GetTamanho()-1) id = indice[i].GetProxID(id);
        }
        
    }

    // Imprime a tabela com os pesos
    /*for (int i = 0; i < num_arq; i++) {
        for (int j = 0; j < tam_vocabulario; j++) {
            cout << "Doc ID: " << tab_peso[i][j].id << " peso do termo: " << tab_peso[i][j].conteudo << endl;
        }
        cout << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl;
    }*/

    // Calcula Wd
    Similaridade Wd[num_arq];
    float peso = 0.0;
    int k = 0;
    bool adicionado = false;

    for (int i = 0; i < num_arq; i++) {

        for (int j = 0; j < tam_vocabulario; j++) {
            
            peso = tab_peso[i][j].conteudo;
            for (k = 0; k < num_arq; k++) { 
                if (Wd[k].id == tab_peso[i][j].id) {
                    adicionado = true;
                    Wd[k].conteudo += peso * peso;
                    break;
                }
            }
            if (!adicionado) {
                for ( k = 0; k < num_arq; k++) {
                    if (Wd[k].id == -1) {
                        Wd[k].id = tab_peso[i][j].id;
                        Wd[k].conteudo += peso * peso;
                        break;
                    }
                }
            }
            adicionado = false;
        }
    } 

    for (int i = 0; i < num_arq; i++) {
        Wd[i].conteudo = sqrt(Wd[i].conteudo);
    }

    // Imprime os Wd
    /*for (int i = 0; i < num_arq; i++) {
        cout << "Doc ID: " << Wd[i].id << " Doc wd: " << Wd[i].conteudo << endl;
    }*/
    
    // Le os termos que compoem a consulta
    string consulta;
    string aux;
    FilaEncadeada termos_consulta;
    ifstream consulta_arquivo;

    consulta_arquivo.open(arq_consulta);
    if (!consulta_arquivo) {
        cout << "Arquivo de consulta nao pode ser aberto" << endl;
        abort();
    }

    getline(consulta_arquivo, consulta);

    if (consulta != "") {
        char * c_consulta = new char [consulta.length()+1];
        strcpy (c_consulta, consulta.c_str());

        // c_consulta eh uma copia de consulta

        char * p = strtok (c_consulta," ");
        while (p!=0) {
            aux = p;
            termos_consulta.Enfileira(aux);

            p = strtok(NULL," ");
        }
        delete[] c_consulta;
    }
    else {
        cout << "Arquivo consulta esta vazio" << endl;
        return;
    }

    consulta_arquivo.close();

    // Define o peso dos termos na consulta
    int q[tam_vocabulario];
    for (int i = 0; i < tam_vocabulario; i++) q[i] = 0;
    
    int tam = 0;
    tam = termos_consulta.GetTamanho();

    for (int i = 0; i < tam; i++) {

        aux = termos_consulta.Desenfileira();
        termos_consulta.Enfileira(aux);

        for (int j = 0; j < tam_vocabulario; j++) {
            
            if (indice[j].GetTermo() == aux) {
                q[j] = 1;
                break;
            }
        }
    } 

    // Similaridade
    Similaridade similaridade[num_arq];
    FilaEncadeada pos;
    int posicao = 0;

    // Guarda a posicao dos termos consultados na tabela
    for (int i = 0; i < tam; i++) {
        for (int j = 0; j < tam_vocabulario; j++) {
            if (q[j]) {
                pos.Enfileira(to_string(j));
            }
        }
    }    

    // Faz a soma dos pesos dos termos nos documentos
    adicionado = false;
    for (int i = 0; i < tam; i++) {
        posicao = stoi(pos.Desenfileira());
        pos.Enfileira(to_string(posicao));
        if (!i) {
            for (int j = 0; j < num_arq; j++) {
                similaridade[j].id = tab_peso[j][posicao].id;
                similaridade[j].conteudo = tab_peso[j][posicao].conteudo;
            }
        }
        else {
            for (int j = 0; j < num_arq; j++) {
                for (k = 0; k < num_arq; k++) {
                    if (similaridade[k].id == tab_peso[j][posicao].id) {
                        similaridade[k].conteudo += tab_peso[j][posicao].conteudo;
                        adicionado = true;
                        break;
                    }
                }
                if (!adicionado) {
                    for (k = 0; k < num_arq; k++) {
                        if (similaridade[k].id == -1) {
                            similaridade[k].id = tab_peso[j][posicao].id;
                            similaridade[k].conteudo = tab_peso[j][posicao].conteudo;
                            break;
                        }
                    }
                }
                adicionado = false;
            }
        }   
    }
    
    // Faz a divisao da soma dos pesos dos termos no documento pelo Wd
    for (int i = 0; i < num_arq; i++) {
        for (int j = 0; j < num_arq; j++) {
            if (similaridade[i].id == Wd[j].id) {
                similaridade[i].conteudo /= Wd[j].conteudo;
            }  
        }
    }

    // Imprime as similaridades
    /*for (int i = 0; i < num_arq; i++) {
        cout << "Doc ID: " << similaridade[i].id << " similaridade: " << similaridade[i].conteudo << endl;
    }*/

    // Ordena Similaridade
    for (int i = 0; i < num_arq; ++i) {
        int maior = i;
        for (int seguinte = i+1; seguinte < num_arq; ++seguinte) {
            if (similaridade[seguinte].conteudo > similaridade[maior].conteudo) maior = seguinte;
            else if (similaridade[seguinte].conteudo == similaridade[maior].conteudo && similaridade[seguinte].id < similaridade[maior].id) maior = seguinte;
            
        }
        Similaridade auxiliar = similaridade[i];
        similaridade[i] = similaridade[maior];
        similaridade[maior] = auxiliar;
    }

    // Imprime resultado
    ofstream ranking;

    ranking.open(arq_ranking);
    if (!ranking) {
        cout << "Arquivo ranking nao pode ser aberto" << endl;
        abort();
    }
    
    for (int i = 0; i < 10; i++) {
        if(similaridade[i].conteudo) {
            if (i < 9) 
                ranking << similaridade[i].id << " ";
            else
                ranking << similaridade[i].id;
        }
    }

    ranking.close();

    // Desaloca a matriz
    for (int i = 0; i < num_arq; i++) {
        delete[] tab_peso[i];
    }
    delete[] tab_peso;
}