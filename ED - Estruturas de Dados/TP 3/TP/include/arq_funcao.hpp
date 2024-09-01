#ifndef ARQ_FUNCAO
#define ARQ_FUNCAO

#include "arq.hpp"
#include "filaencadeada.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <dirent.h>
#include <cstdio>

using namespace std;

// Confere se o arquivo esta vazio
bool ArquivoVazio(string nome_arq);

// Conta quantas linhas tem no arquivo
int ContaLinhas(string nome_arq_entrada);

// Conta quantas palavras tem no arquivo
    // Incluindo palavras repetidas
int ContaPalavrasArq(string nome_arq_entrada);

// Conta o numero de arquivos de uma pasta
int ContaArquivos(string nome_pasta);

// Conta o numero de palavras de uma pasta
int ContaPalavrasPasta(string nome_pasta, string arq_stop_words);

// Cria copia do arquivo
void CriaCopia(string nome_arq, string copia);

// Apaga primeira linha do arquivo
void ApagaPrimeiraLinhaArquivo(string nome_arq);

// Passa todas as letras do arquivo para letras minusculas
void LetrasMinusculas(string nome_arq);

// Substitui todos os caracteres diferentes de letras por espacos
void SubstituiPorEspaco(string nome_arq);

// Exclui todos os termos com numeros no arquivo
void ExcluiNumeros(string nome_arq);

// Exclui todas as stopwords
void ExcluiStopWords(string nome_arq, string arq_stop_words);

// Prepara o arquivo para o codigo
void PreparaArquivo(string nome_arq);

// Prepara o arquivo para o codigo
    // Ja exclui as stopwords
void PreparaArquivo(string nome_arq, string arq_stop_words);

// Prepara os arquivos para o codigo
//void PreparaCorpus(string nome_pasta);

// Prepara os arquivos para o codigo
    // Ja exclui as stopwords
int PreparaCorpus(string nome_pasta, string arq_stop_words);

#endif