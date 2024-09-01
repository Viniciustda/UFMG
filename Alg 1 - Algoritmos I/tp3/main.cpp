#include <iostream>
#include <vector>

#define INF 0x3f3f3f3f

using namespace std;

int MinimoLigas(vector<int> tamanhos, int demanda) {

  // Armazena a quantidade minima de ligas necessarias para atender as diferentes demandas
  vector<int> resposta(demanda+1, 0);
  
  // Numero de tamanhos de ligas disponiveis 
  int num_tam = tamanhos.size();
  
  // Calcula o numero minimo de ligas necessarias para atender a cada demanda, de 1 ate a demanda especificada
  for (int i = 1; i < demanda+1; i++) {
    int minimo = INF;

    // Se o tamanho da liga disponivel for menor ou igual a demanda, calcula o valor minimo para atender a demanda atual
    for (int j = 0; j < num_tam; j++) {
      if (tamanhos[j] <= i)
        minimo = min(minimo, resposta[i-tamanhos[j]]);
    }

    // Atualiza a resposta para a demanda atual
    resposta[i] = minimo + 1;
  }

  // Retorna a quantidade minima de ligas necessarias para atender a demanda
  return resposta[demanda];
}

int main(int argc, char const *argv[]) {

  // Variaveis
  int testes, num_tam, demanda;
  vector <int> tamanhos;

  // Recebe o numero de testes
  cin >> testes;

  // Executa cada teste
  for (int i = 0; i < testes; i++) {
    // Recebe o numero de tamanhos de ligas disponiveis 
    cin >> num_tam >> demanda;

    // Armazena os tamanhos disponiveis
    tamanhos.resize(num_tam);
    for (int j = 0; j < num_tam; j++) 
      cin >> tamanhos[j];
        
    // Imprime a quantidade minima de ligas necessarias para atender a demanda
    cout << MinimoLigas(tamanhos, demanda) << endl;
  }

  return 0;
}