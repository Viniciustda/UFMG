#include "arq.hpp"
#include "arq_funcao.hpp"
#include "filaencadeada.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <dirent.h>
#include <cstdio>

using namespace std;

// Confere se o arquivo esta vazio
bool ArquivoVazio(string nome_arq) {
    bool vazio = false;
    string linha;
    int indice = 0;
    ifstream arquivo_saida;
    arquivo_saida.open(nome_arq);

    while(!arquivo_saida.eof()) {
        getline(arquivo_saida, linha);
        indice++;
        if (indice > 1)
            break;
    }
    if (indice == 1 && linha == "") 
        vazio = true;
    
    arquivo_saida.close();
    return vazio;
}

// Conta o numero de linhas do arquivo
int ContaLinhas(string nome_arq_entrada) {
    string linha;
    int count = 0;

    // Abre o arquivo de entrada
    ifstream arquivo_entrada;
    arquivo_entrada.open(nome_arq_entrada);
    if (! arquivo_entrada) {
        cout << "Arquivo de entrada (1) nao pode ser aberto" << endl;
        abort();
    }

    // Conta quantas linhas tem no arquivo
    while (!arquivo_entrada.eof()) {
        getline(arquivo_entrada, linha);
        if (linha != "")
            count++;
    }
        
    // Fecha o arquivo
    arquivo_entrada.close();

    return count;
}

// Conta o numero de palavras no arquivo
    // Incluindo palavras repetidas
int ContaPalavrasArq(string nome_arq_entrada) {
    string linha;
    int count = 0;

    // Abre o arquivo de entrada
    ifstream arquivo_entrada;
    arquivo_entrada.open(nome_arq_entrada);
    if (! arquivo_entrada) {
        cout << "Arquivo de entrada (2) nao pode ser aberto" << endl;
        abort();
    }

    // Conta quantas palavras tem no arquivo
    while (!arquivo_entrada.eof()) {

        getline(arquivo_entrada, linha);

        if (linha != "") {
            char * clinha = new char [linha.length()+1];
            strcpy (clinha, linha.c_str());

            // clinha eh uma copia de linha

            char * p = strtok (clinha," ");
            while (p!=0) {
                count++;
                p = strtok(NULL," ");
            }
            
            delete[] clinha;
        }
    }
        
    // Fecha o arquivo
    arquivo_entrada.close();

    return count;
}

// Conta o numero de arquivos de uma pasta
int ContaArquivos(string nome_pasta) {

    int num_arq = -2; // Comeca com -2 pra nao contar os diretorios da pasta

    DIR *dir;
    struct dirent *dir_aux;

    dir = opendir(nome_pasta.c_str());

    while ( ( dir_aux = readdir(dir) ) != NULL ) num_arq++;

    closedir(dir);

    return num_arq;
}

// Conta o numero de palavras de uma pasta
int ContaPalavrasPasta(string nome_pasta, string arq_stop_words) {

    // Descobre o nome dos arquivos
    DIR *dir;
    struct dirent *dir_aux;
    FilaEncadeada nome_arquivos;

    dir = opendir(nome_pasta.c_str());

    dir_aux = readdir(dir);    // Pula os diretorios da pasta
    dir_aux = readdir(dir);

    while ( ( dir_aux = readdir(dir) ) != NULL ) nome_arquivos.Enfileira(dir_aux->d_name);

    closedir(dir);

    // Variaveis para contar as palavras
    FilaEncadeada vocabulario;
    string str_aux;
    string str_aux2;
    string copia;
    string aux;
    int count = 0;
    bool existe = false;
    int total = 0;
    
    while (nome_arquivos.GetTamanho()) {

        aux = nome_arquivos.Desenfileira();
        if (aux == "18636.txt") cout << "ultimo arq" << endl;
        

        //copia = nome_pasta+"/copia-"+aux;
        aux = nome_pasta+"/"+aux;

        //CriaCopia(aux, copia);

        PreparaArquivo(aux, arq_stop_words);
    
        // Abre o arquivo de entrada
        fstream arquivo_entrada;
        arquivo_entrada.open(aux);
        if (! arquivo_entrada) {
            cout << "Arquivo de entrada (3) nao pode ser aberto" << endl;
            abort();
        }

        // Conta quantas palavras tem no arquivo
        while (!arquivo_entrada.eof()) {

            getline(arquivo_entrada, aux);

            if (aux != "") {
                char * c_aux = new char [aux.length()+1];
                strcpy (c_aux, aux.c_str());

                char * p = strtok (c_aux," ");
                while (p!=0) {

                    str_aux = p;
                    count = vocabulario.GetTamanho();
                    
                    if (!vocabulario.GetTamanho()) {
                        vocabulario.Enfileira(str_aux);
                        total++;
                    }
                    else {
                        for (int i = 0; i < count; i++) {
                            str_aux2 = vocabulario.Desenfileira();
                            if (str_aux == str_aux2) {
                                existe = true;
                                vocabulario.Enfileira(str_aux2);
                                break;
                            }
                            vocabulario.Enfileira(str_aux2);
                            
                        }
                        if (!existe) {
                            vocabulario.Enfileira(str_aux);
                            total++;
                        }
                        existe = false;
                          
                    }

                    p = strtok(NULL," ");
                }
                
                delete[] c_aux;
            }
        }
            
        // Fecha e apaga o arquivo
        arquivo_entrada.close();
        //remove(copia.c_str());

    }
    return total;

}

// Conta o numero de arquivos de uma pasta - mas pode dar problema facilmente porque depende do nome dos arquivos
/*
int ContaArquivos(string nome_pasta) {

    ifstream arquivo_entrada;
    string nome_arq;
    int num_arq = 0;
    
    while (1) {
        
        nome_arq = nome_pasta + "/" + to_string(num_arq) + ".txt";
        
        arquivo_entrada.open(nome_arq);
        if (arquivo_entrada) {
            num_arq++;
            arquivo_entrada.close();
        }
        else{
            break;
        }
    }

    return num_arq;
}  
*/

// Cria copia do arquivo
void CriaCopia(string nome_arq, string copia) {

    FilaEncadeada linhas;
    string entrada;
    string saida;

    // Abre o arquivo para leitura
    ifstream arquivo_entrada;
    arquivo_entrada.open(nome_arq);
    if (! arquivo_entrada) {
        cout << "Arquivo de entrada (4) nao pode ser aberto" << endl;
        abort();
    }

    // Le as linhas do arquivo que sera copiado
    while (!arquivo_entrada.eof()) {
        getline(arquivo_entrada, entrada);
        if (entrada != "")
            linhas.Enfileira(entrada);
    }

    // Fecha o arquivo
    arquivo_entrada.close();

    // Abre o arquivo copia para escrita
    ofstream arquivo_saida;
    arquivo_saida.open(copia);
    if (! arquivo_saida) {
        cout << "Arquivo de saida nao pode ser aberto (1)" << endl;
        abort();
    }

    // Escreve no arquivo todas as linhas que ja estvam no arquivo, com excecao da primeira
    while (!linhas.Vazia()) {
        saida = linhas.Desenfileira();
        if (saida != " " && saida != "\n" && saida != "\0")
            arquivo_saida << saida << endl;
    }
    
    // Fecha o arquivo
    arquivo_saida.close();

}

// Apaga primeira linha do arquivo
void ApagaPrimeiraLinhaArquivo(string nome_arq) {

    FilaEncadeada linhas;
    string entrada;
    string saida;

    // Abre o arquivo para leitura
    ifstream arquivo_entrada;
    arquivo_entrada.open(nome_arq);
    if (! arquivo_entrada) {
        cout << "Arquivo de entrada (5) nao pode ser aberto" << endl;
        abort();
    }

    // Le as linhas do arquivo e, comecando pela segunda linha, adiciona numa fila encadeada
    getline(arquivo_entrada, entrada);
    while (!arquivo_entrada.eof()) {
        getline(arquivo_entrada, entrada);
        if (entrada != "")
            linhas.Enfileira(entrada);
    }

    // Fecha o arquivo
    arquivo_entrada.close();

    // Abre o arquivo para escrita
    ofstream arquivo_saida;
    arquivo_saida.open(nome_arq);
    if (! arquivo_saida) {
        cout << "Arquivo de saida nao pode ser aberto (2)" << endl;
        abort();
    }

    // Escreve no arquivo todas as linhas que ja estvam no arquivo, com excecao da primeira
    while (!linhas.Vazia()) {
        saida = linhas.Desenfileira();
        if (saida != " " && saida != "\n" && saida != "\0")
            arquivo_saida << saida << endl;
    }
    
    // Fecha o arquivo
    arquivo_saida.close();

}

// Passa todas as letras do arquivo para letras minusculas
void LetrasMinusculas(string nome_arq) {

    FilaEncadeada linhas;
    string entrada;
    string saida;
    int tam = 0;

    // Abre o arquivo para leitura
    ifstream arquivo_entrada;
    arquivo_entrada.open(nome_arq);
    if (! arquivo_entrada) {
        cout << "Arquivo de entrada (6) nao pode ser aberto" << endl;
        abort();
    }

    // Le as linhas do arquivo e passa para letras minusculas
    while (!arquivo_entrada.eof()) {
        getline(arquivo_entrada, entrada);
        if (entrada != "") {

            char * c_entrada = new char [entrada.length()+1];
            strcpy (c_entrada, entrada.c_str());

            // c_entrada eh uma copia de entrada

            tam = entrada.length();
            for (int i = 0; i <  tam; i++) {
                if (c_entrada[i] >= 65 && c_entrada[i] <= 90) 
                    c_entrada[i] += 32;
            }

            entrada = c_entrada;
            
            delete[] c_entrada;

            linhas.Enfileira(entrada);
        }
    }

    // Fecha o arquivo
    arquivo_entrada.close();

    // Abre o arquivo para escrita
    ofstream arquivo_saida;
    arquivo_saida.open(nome_arq);
    if (! arquivo_saida) {
        cout << "Arquivo de saida nao pode ser aberto (3)" << endl;
        abort();
    }

    // Escreve no arquivo todas as linhas, ja com as letras minusculas
    while (!linhas.Vazia()) {
        saida = linhas.Desenfileira();
        if (saida != " " && saida != "\n" && saida != "\0")
            arquivo_saida << saida << " ";
    }
    
    // Fecha o arquivo
    arquivo_saida.close();

}

// Substitui todos os caracteres diferentes de letras por espacos
void SubstituiPorEspaco(string nome_arq) {

    FilaEncadeada linhas;
    string entrada;
    string saida;
    int tam = 0;

    // Abre o arquivo para leitura
    ifstream arquivo_entrada;
    arquivo_entrada.open(nome_arq);
    if (! arquivo_entrada) {
        cout << "Arquivo de entrada (7) nao pode ser aberto" << endl;
        abort();
    }

    // Le as linhas do arquivo e substitui por espacos
    while (!arquivo_entrada.eof()) {
        getline(arquivo_entrada, entrada);
        if (entrada != "") {

            char * c_entrada = new char [entrada.length()+1];
            strcpy (c_entrada, entrada.c_str());

            tam = entrada.length();
            for (int i = 0; i < tam; i++) {
                if (c_entrada[i] == 92) 
                    c_entrada[i+1] = ' ';
                
                if (!(c_entrada[i] >= 97 && c_entrada[i] <= 122) && !(c_entrada[i] >= 48 && c_entrada[i] <= 57) && c_entrada[i] != ' ') 
                    c_entrada[i] = ' ';
            }

            entrada = c_entrada;
            
            delete[] c_entrada;

            // Se ficar com mais de um espaco seguido, apaga ate ficar so um espaco 
            tam = entrada.length();
            for ( int i = 0; i < tam; i++) {
                if (entrada[i] == ' ' && entrada[i+1] == ' ') {
                    entrada.erase(i,1);
                    i--;
                }
                
            }
            

            linhas.Enfileira(entrada);
        }
    }

    // Fecha o arquivo
    arquivo_entrada.close();

    // Abre o arquivo para escrita
    ofstream arquivo_saida;
    arquivo_saida.open(nome_arq);
    if (! arquivo_saida) {
        cout << "Arquivo de saida nao pode ser aberto (4)" << endl;
        abort();
    }

    // Escreve no arquivo todas as linhas, ja com as substituicoes feitas
    while (!linhas.Vazia()) {
        saida = linhas.Desenfileira();
        if (saida != " " && saida != "\n" && saida != "\0")
            arquivo_saida << saida << " ";
    }
    
    // Fecha o arquivo
    arquivo_saida.close();

}

// Exclui todos os termos com numeros no arquivo
void ExcluiNumeros(string nome_arq) {

    FilaEncadeada linhas;
    string entrada;
    string saida;
    bool contem_numero = false;
    int indice = 0;

    // Abre o arquivo para leitura
    ifstream arquivo_entrada;
    arquivo_entrada.open(nome_arq);
    if (! arquivo_entrada) {
        cout << "Arquivo de entrada (8) nao pode ser aberto" << endl;
        abort();
    }

    // Le as linhas do arquivo e exclui os termos que contem numeros
    while (!arquivo_entrada.eof()) {
        getline(arquivo_entrada, entrada);
        if (entrada != "") {

            char * c_entrada = new char [entrada.length()+1];
            strcpy (c_entrada, entrada.c_str());

            entrada = "";

            // c_entrada eh uma copia de entrada

            char * p = strtok (c_entrada," ");
            while (p) {
                indice = 0;
                while (p[indice]) {
                    if (p[indice] >= 48 && p[indice] <= 57)
                        contem_numero = true;
                    indice++;
                }
                if (!contem_numero) {
                    entrada += p;
                    entrada += " ";
                }
                else {
                    contem_numero = false;
                }

                p = strtok(NULL," ");
            }
            
            delete[] c_entrada;

            if (entrada.length())
                entrada.pop_back();

            if (entrada.length())
                linhas.Enfileira(entrada);
        }
    }

    // Fecha o arquivo
    arquivo_entrada.close();

    // Abre o arquivo para escrita
    ofstream arquivo_saida;
    arquivo_saida.open(nome_arq);
    if (! arquivo_saida) {
        cout << "Arquivo de saida nao pode ser aberto (5)" << endl;
        abort();
    }

    // Escreve no arquivo todas as linhas, sem os termos com numeros
    while (!linhas.Vazia()) {
        saida = linhas.Desenfileira();
        if (saida != " " && saida != "\n" && saida != "\0")
            arquivo_saida << saida << " ";
    }
    
    // Fecha o arquivo
    arquivo_saida.close();
}

// Exclui todas as stopwords
void ExcluiStopWords(string nome_arq, string arq_stop_words) {

    string entrada;
    string saida;
    string str_aux;

    // Stopwords
    FilaEncadeada stopwords;
    
    // Abre o arquivo de entrada
    ifstream stop_words;
    stop_words.open(arq_stop_words);
    if (! stop_words) {
        cout << "stop_words nao pode ser aberto (2)" << endl;
        abort();
    }
    
    while (!stop_words.eof()) {

        getline(stop_words, entrada);

        if (entrada != "") {
            char * centrada = new char [entrada.length()+1];
            strcpy (centrada, entrada.c_str());

            char * p = strtok (centrada," ");
            while (p!=0) {
                str_aux = p;
                stopwords.Enfileira(str_aux);
                p = strtok(NULL," ");
            }
                
            delete[] centrada;
        }
    }
    
    stop_words.close();

    // Variaveis para contar as palavras
    FilaEncadeada vocabulario;  
    
    // Abre o arquivo de entrada
    ifstream arquivo_entrada;
    arquivo_entrada.open(nome_arq);
    if (! arquivo_entrada) {
        cout << "Arquivo de entrada (9) nao pode ser aberto" << endl;
        abort();
    }

    // Conta quantas palavras tem no arquivo
    while (!arquivo_entrada.eof()) {

        getline(arquivo_entrada, entrada);

        if (entrada != "") {
            char * c_entrada = new char [entrada.length()+1];
            strcpy (c_entrada, entrada.c_str());

            char * p = strtok (c_entrada," ");
            while (p!=0) {

                str_aux = p;
                vocabulario.Enfileira(str_aux);

                p = strtok(NULL," ");
            }
                
            delete[] c_entrada;
        }
    }
            
    // Fecha o arquivo
    arquivo_entrada.close();

    // Abre o arquivo para escrita
    bool existe = false;
    ofstream arquivo_saida;
    arquivo_saida.open(nome_arq);
    if (! arquivo_saida) {
        cout << "Arquivo de saida nao pode ser aberto (6)" << endl;
        abort();
    }

    // Escreve no arquivo todas as linhas, ja com as letras minusculas
    while (!vocabulario.Vazia()) {
        saida = vocabulario.Desenfileira();
        if (saida != " " && saida != "\n" && saida != "\0") {

            for (int i = 0; i < stopwords.GetTamanho() ; i++) {
                str_aux = stopwords.Desenfileira();
                if (saida == str_aux) {
                    existe = true;
                    stopwords.Enfileira(str_aux);
                    break;                            
                }
                    stopwords.Enfileira(str_aux);
                            
            }
            if (!existe) {
                arquivo_saida << saida << " ";
            }
            existe = false;
            
        }
    }
    
    // Fecha o arquivo
    arquivo_saida.close();

}

// Prepara o arquivos para o codigo 
void PreparaArquivo(string nome_arq) {

    LetrasMinusculas(nome_arq);
    SubstituiPorEspaco(nome_arq);
    ExcluiNumeros(nome_arq);

}

// Prepara o arquivos para o codigo 
    // Ja exclui as stopwords
void PreparaArquivo(string nome_arq, string arq_stop_words) {

    //cout << "Letras Minusculas " << nome_arq << ": ";
    LetrasMinusculas(nome_arq);
    //cout << "OK" << endl;
    //cout << "Substitui po espaco " << nome_arq << ": ";
    SubstituiPorEspaco(nome_arq);
    //cout << "OK" << endl;
    //cout << "Exclui numeros " << nome_arq << ": ";
    ExcluiNumeros(nome_arq);
    //cout << "OK" << endl;
    //cout << "Exclui stopwords " << nome_arq << ": ";
    ExcluiStopWords(nome_arq, arq_stop_words);
    //cout << "OK" << endl;

}

// Prepara os arquivos para o codigo
/*void PreparaCorpus(string nome_pasta) {

    string nome_arq;

    DIR *dir;
    struct dirent *dir_aux;

    dir = opendir(nome_pasta.c_str());

    dir_aux = readdir(dir); // Esta operacao eh realizada duas vezes para pular os diretorios da pasta
    dir_aux = readdir(dir);

    while ( ( dir_aux = readdir(dir) ) != NULL ) {
        nome_arq = dir_aux->d_name;
        PreparaArquivo(nome_arq);
    }

    closedir(dir);

}*/

// Prepara os arquivos para o codigo
    // Ja exclui as stopwords

int PreparaCorpus(string nome_pasta, string arq_stop_words) {

    // Contador para saber quantos arquivos ja foram tratados
    int tratados = 0;

    // Descobre o nome dos arquivos
    DIR *dir;
    struct dirent *dir_aux;
    FilaEncadeada nome_arquivos;

    dir = opendir(nome_pasta.c_str());

    dir_aux = readdir(dir);    // Pula os diretorios da pasta
    dir_aux = readdir(dir);

    while ( ( dir_aux = readdir(dir) ) != NULL ) nome_arquivos.Enfileira(dir_aux->d_name);

    closedir(dir);

    // Variaveis para contar as palavras
    FilaEncadeada vocabulario;
    FilaEncadeada vocabulario_tratado;
    FilaEncadeada vocabulario_final;
    string nome;
    string str_aux;
    string str_aux2;
    string aux;
    int count = 0;
    bool existe = false;
    FilaEncadeada linhas;
    string saida;
    int tam = 0;
    int indice = 0;
    FilaEncadeada stopwords;

    // Stopwordsd
    ifstream stop_words;
    stop_words.open(arq_stop_words);
    if (! stop_words) {
        cout << "stop_words nao pode ser aberto" << endl;
        abort();
    } 
    while (!stop_words.eof()) {

        getline(stop_words, aux);

        if (aux != "") {
            char * c_aux = new char [aux.length()+1];
            strcpy (c_aux, aux.c_str());

            char * p = strtok (c_aux," ");
            while (p!=0) {
                str_aux = p;
                stopwords.Enfileira(str_aux);
                p = strtok(NULL," ");
            }
                
            delete[] c_aux;
        }
    }
    stop_words.close();
    
    while (nome_arquivos.GetTamanho()) {

        nome = nome_arquivos.Desenfileira();
        nome = nome_pasta+"/"+nome;
    
        // Abre o arquivo de entrada
        ifstream arquivo_e;
        arquivo_e.open(nome);
        if (! arquivo_e) {
            cout << "Arquivo de entrada nao pode ser aberto" << endl;
            abort();
        }

        while (!arquivo_e.eof()) {

            getline(arquivo_e, aux);            

            if (aux != "") {

                // Trata letras maiusculas
                tam = aux.length();
                for (int i = 0; i <  tam; i++) {
                                
                    if (aux[i] >= 65 && aux[i] <= 90) 
                        aux[i] += 32;
                
                    if (aux[i] == 92) 
                        aux[i+1] = ' ';
                                        
                    if (!(aux[i] >= 97 && aux[i] <= 122) && !(aux[i] >= 48 && aux[i] <= 57) && aux[i] != ' ') 
                        aux[i] = ' ';
                                        
                    if ( (aux[i] == ' ' && aux[i+1] == ' ') || (aux[i] == ' ' && aux[i-1] == ' ') ) {
                        aux.erase(i,1);
                        tam--;
                        i--;
                    }
                }

                // Exclui termos com numeros
                char * c_aux = new char [aux.length()+1];
                strcpy (c_aux, aux.c_str());
                aux = "";
                char * p = strtok (c_aux," ");
                while (p) {
                    indice = 0;
                    while (p[indice]) {
                        if (p[indice] >= 48 && p[indice] <= 57)
                            existe = true;
                        indice++;
                    }
                    if (!existe) {
                        aux += p;
                        aux += " ";
                    }
                    else {
                        existe = false;
                    }
                    p = strtok(NULL," ");
                }
                if (aux.length())
                    aux.pop_back();

                // Exclui stopwords
                strcpy (c_aux, aux.c_str());
                char * q = strtok (c_aux," ");
                while (q!=0) {
                    str_aux = q;
                    vocabulario.Enfileira(str_aux);
                    q = strtok(NULL," ");
                }
                aux = "";
                existe = false;
                while (!vocabulario.Vazia()) {
                    saida = vocabulario.Desenfileira();
                    if (saida != " " && saida != "\n" && saida != "\0") {

                        for (int i = 0; i < stopwords.GetTamanho() ; i++) {
                            str_aux = stopwords.Desenfileira();
                            if (saida == str_aux) {
                                existe = true;
                                stopwords.Enfileira(str_aux);
                                break;                            
                            }
                                stopwords.Enfileira(str_aux);         
                        }
                        if (!existe) {
                            aux += saida;
                            aux += " ";
                        }
                        existe = false;
                    }
                }
                if (aux.length()) {
                    aux.pop_back();
                    vocabulario_final.Enfileira(aux);
                }

                // Conta as palavras
                strcpy (c_aux, aux.c_str());
                char * r = strtok (c_aux," ");
                while (r!=0) {

                    str_aux = r;
                    count = vocabulario_tratado.GetTamanho();
                    
                    if (!vocabulario_tratado.GetTamanho()) {
                        vocabulario_tratado.Enfileira(str_aux);
                    }
                    else {
                        for (int i = 0; i < count; i++) {
                            str_aux2 = vocabulario_tratado.Desenfileira();
                            if (str_aux == str_aux2) {
                                existe = true;
                                vocabulario_tratado.Enfileira(str_aux2);
                                break;
                            }
                            vocabulario_tratado.Enfileira(str_aux2);
                            
                        }
                        if (!existe) {
                            vocabulario_tratado.Enfileira(str_aux);
                        }
                        existe = false;
                          
                    }
                    r = strtok(NULL," ");
                }

                delete[] c_aux;
            }
        }
            
        // Fecha o arquivo
        arquivo_e.close();

        ofstream arquivo_s;
        arquivo_s.open(nome);
        if (! arquivo_s) {
            cout << "Arquivo de entrada nao pode ser aberto." << endl;
            abort();
        }

        // Escreve no arquivo todas a linha, ja tratada
        while (!vocabulario_final.Vazia()) {
            saida = vocabulario_final.Desenfileira();
            arquivo_s << saida << " ";
        }

        // Fecha o arquivo
        arquivo_s.close();

        // A cada 100 arquivos tratados eh impresso quantos arquivos foram tratados ao todo, permitindo que seja acompanhada a execucao do programa
        tratados++;
        if (tratados % 200 == 0) cout << "Arquivos tratados ate o momento: " << tratados << endl;
    }
    return vocabulario_tratado.GetTamanho();
}

/*void PreparaCorpus(string nome_pasta, string arq_stop_words) {

    string nome_arq;

    DIR *dir;
    struct dirent *dir_aux;

    dir = opendir(nome_pasta.c_str());

    dir_aux = readdir(dir); // Esta operacao eh realizada duas vezes para pular os diretorios da pasta
    dir_aux = readdir(dir);

    while ( ( dir_aux = readdir(dir) ) != NULL ) {
        nome_arq = dir_aux->d_name;
        PreparaArquivo(nome_arq, arq_stop_words);
    }

    closedir(dir);

} */
