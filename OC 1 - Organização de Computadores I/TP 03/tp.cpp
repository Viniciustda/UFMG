#include <iostream>
#include <string>
#include <fstream>
#include <bitset>
#include <vector>
#include <cmath>
#include <queue>
#include <iomanip>

using namespace std;

void MapeamentoDireto(string nome_arq_entrada, int num_linhas) {

  // Declara variaveis
  string dado_entrada;
  long long int dado;
  int indice_d;
  bitset<32> binario;
  bitset<32> indice_b;
  vector<long long int> cache(num_linhas);
  vector<long long int> validade(num_linhas, 0);
  int hit = 0;
  int miss = 0;
  bool verifica_hit = false;

  // Abre os arquivo
  ifstream arquivo_entrada(nome_arq_entrada);
  ofstream arquivo_saida("output.txt");

  // Verifica se os arquivos foram abertos corretamente
  if (!arquivo_entrada.is_open()) 
    cerr << "Erro ao abrir o arquivo: " << nome_arq_entrada << endl;

  if (!arquivo_saida.is_open()) 
    cerr << "Erro ao abrir o arquivo: " << "output.txt" << endl;

  // Le o conteudo do arquivo de entrada linha por linha
  while (getline(arquivo_entrada, dado_entrada)) {

    // Identifica indice e tag
    dado = stoll(dado_entrada, nullptr, 16);
    binario = dado;
    bitset<32> mascara((1u << static_cast<int>(ceil(log2(num_linhas)))) - 1);
    indice_b = binario & mascara;
    indice_d = indice_b.to_ullong();
    binario  >>= log2(num_linhas);
    dado = binario.to_ullong();

    // Verifica hits e miss
    if (validade[indice_d] == 1) {
      if (cache[indice_d] == dado) {
        hit++;
        verifica_hit = true;
      }
      else {
        miss++;
        cache[indice_d] = dado;
      }
    }
    else {
      miss++;
      cache[indice_d] = dado;
      validade[indice_d] = 1;
    }

    // Imprime saida
    if (verifica_hit == false) {
      arquivo_saida << "================" << endl << "IDX V ** ADDR **" << endl;
      for (int i = 0; i < num_linhas; i++) {
        if (validade[i])
          arquivo_saida << setfill('0') << setw(3) << i << " " << validade[i] << " " << "0x" << setfill('0') << setw(8) << hex << uppercase << cache[i] << endl;
        else
          arquivo_saida << setfill('0') << setw(3) << i << " " << validade[i] << endl;
      }
    }

    // Reinicia variavel
    verifica_hit = false;

  }

  // Imprime hits e miss
  arquivo_saida << endl << "#hits: " << hit << endl;
  arquivo_saida << "#miss: " << miss;

  // Fecha os arquivos
  arquivo_entrada.close();
  arquivo_saida.close();

}

void AssociatividadeCompleta(int tam_linha, int tam_grupo, string nome_arq_entrada) {

  // Declara variaveis
  string dado_entrada;
  long long int dado;
  bitset<32> binario;
  vector<long long int> cache(tam_grupo);
  vector<long long int> validade(tam_grupo, 0);
  int hit = 0;
  int miss = 0;
  bool verifica_hit = false;
  bool vazio = false;
  queue<long long int> fila;

  // Abre os arquivo
  ifstream arquivo_entrada(nome_arq_entrada);
  ofstream arquivo_saida("output.txt");

  // Verifica se os arquivos foram abertos corretamente
  if (!arquivo_entrada.is_open()) 
    cerr << "Erro ao abrir o arquivo: " << nome_arq_entrada << endl;

  if (!arquivo_saida.is_open()) 
    cerr << "Erro ao abrir o arquivo: " << "output.txt" << endl;

  // Le o conteudo do arquivo de entrada linha por linha
  while (getline(arquivo_entrada, dado_entrada)) {

    // Ignora bits menos significativos
    dado = stoll(dado_entrada, nullptr, 16);
    binario = dado;
    binario  >>= log2(tam_linha);
    dado = binario.to_ullong();

    // Verifica hits e se existe espaco vazio na cache
    for (int i = 0; i < tam_grupo; i++) {
      if (cache[i] == dado && validade[i] == 1) {
        hit++;
        verifica_hit = true;
        break;
      }
      if (validade[i] == 0) {
        vazio = true;
      }
    }

    // Insere dado na cache 
      // Caso tenha espaco vazio
      // Caso precise usar FIFO
    if (verifica_hit == false) {
      miss++;
      if (vazio) {
        for (int i = 0; i < tam_grupo; i++) {
          if (validade[i] == 0) {
            cache[i] = dado;
            fila.push(dado);
            validade[i] = 1;
            break;
          }
        }
      }
      else {
        long long int aux = fila.front();
        fila.pop();
        for (int i = 0; i < tam_grupo; i++) { 
          if (cache[i] == aux) {
            cache[i] = dado;
            fila.push(dado);
            validade[i] = 1;
            break;
          }
        }
      }
    }

    // Imprime saida
    if (verifica_hit == false) {
      arquivo_saida << "================" << endl << "IDX V ** ADDR **" << endl;
      for (int i = 0; i < tam_grupo; i++) {
        if (validade[i])
          arquivo_saida << setfill('0') << setw(3) << i << " " << validade[i] << " " << "0x" << setfill('0') << setw(8) << hex << uppercase << cache[i] << endl;
        else
          arquivo_saida << setfill('0') << setw(3) << i << " " << validade[i] << endl;
      }
    }

    // Reinicia variaveis
    verifica_hit = false;
    vazio = false;

  }

  // Imprime hits e miss
  arquivo_saida << endl << "#hits: " << hit << endl;
  arquivo_saida << "#miss: " << miss;

  // Fecha os arquivos
  arquivo_entrada.close();
  arquivo_saida.close();

}

void AssociatividadeConjunto(int tam_cache, int tam_linha, int tam_grupo, string nome_arq_entrada, int num_grupos) {

  // Declara variaveis
  string dado_entrada;

  // Abre os arquivo
  ifstream arquivo_entrada(nome_arq_entrada);
  ofstream arquivo_saida("output.txt");

  // Verifica se os arquivos foram abertos corretamente
  if (!arquivo_entrada.is_open()) 
    cerr << "Erro ao abrir o arquivo: " << nome_arq_entrada << endl;

  if (!arquivo_saida.is_open()) 
    cerr << "Erro ao abrir o arquivo: " << "output.txt" << endl;

  // Le o conteudo do arquivo de entrada linha por linha
  while (getline(arquivo_entrada, dado_entrada)) {

  }

  // Fecha os arquivos
  arquivo_entrada.close();
  arquivo_saida.close();

}

int main(int argc, char const *argv[]) {

  // Declara variaveis
  int tam_cache = 0;
  int tam_linha = 0;
  int tam_grupo = 0;
  string nome_arq_entrada;
  int num_linhas = 0;
  int num_grupos = 0;

  // Le argumentos
  tam_cache = stoi(argv[1]);
  tam_linha = stoi(argv[2]);
  tam_grupo = stoi(argv[3]);
  nome_arq_entrada = argv[4];

  // Chama a funcao de acordo com o tipo de associatividade
  num_linhas = tam_cache / tam_linha;
  num_grupos = num_linhas / tam_grupo;

  if (tam_grupo == 1) {MapeamentoDireto(nome_arq_entrada, num_linhas);}
  else if (num_grupos == 1) {AssociatividadeCompleta(tam_linha, tam_grupo, nome_arq_entrada);}
  else {AssociatividadeConjunto(tam_cache, tam_linha, tam_grupo, nome_arq_entrada, num_grupos);}

  return 0;
}