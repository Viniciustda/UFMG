/*
Neste VPL, vocês vão trabalhar com um programa que executa o "Jogo da Vida" desenvolvido por John Horton Conway. Embora não seja um "jogo" no sentido tradicional, ele apresenta um comportamento interessante, especificado com poucas regras. O programa segue as regras da Vida (https://pt.wikipedia.org/wiki/Jogo_da_vida) para mostrar o comportamento contínuo de um conjunto de células em uma matriz. Esta matriz modela um Toro (https://pt.wikipedia.org/wiki/Toro_(topologia)) onde cada célula da matriz (sem exceção) tem exatamente 8 vizinhas. Cada célula da matriz pode estar viva ou morta, e a cada iteração, células mortas podem reviver ou células vivas podem morrer. Os nascimentos e mortes seguem o seguinte conjunto de regras:

- Cada célula possui oito células vizinhas. As vizinhas de uma célula são as células diretamente acima, abaixo, à direita, à esquerda, diagonalmente acima à direita ou à esquerda e diagonalmente abaixo à direita ou à esquerda. 
- Se uma célula ocupada não possui vizinhas ou possui apenas uma, morre de solidão. Se uma célula ocupada possui mais de três vizinhas, morre de superpopulação.

- Se uma célula vazia possui exatamente três células vizinhas ocupadas, há o nascimento de uma nova célula para substituir a célula vazia.

Nascimentos e mortes são instantâneos e ocorrem simultaneamente no início de cada iteração. Uma célula que morre por qualquer razão pode ajudar a provocar o nascimento, mas uma célula recém-nascida não pode ressuscitar uma célula que está morrendo, nem a morte de uma célula impede a morte de outra, digamos, por meio da redução da população local.

Exemplos em: https://www.youtube.com/watch?v=23MBR2pZoDQ

Programa fornecido está quase pronto, mas você tem que realizar as seguintes atividades:


(i) Pesquise e leia sobre o jogo da vida. A primeira coisa a se fazer para entender um programa é entender ao que ele se propõe. 

(ii) O programa contém um erro, que foi propositalmente inserido em uma ÚNICA linha. Sua primeira atividade é, usando GDB,  identificar esta única linha e corrigir o erro alterando apenas esta linha de código. Use também o GDB para acompanhar a execução do código para entender como ele funciona. Somente entendo bem o funcionamento do código, você conseguirá realizar as seguintes tarefas. 

(iii) Sejam N e M o número de linhas e colunas da matriz, respectivamente,  os valores válidos para uma linha i ou coluna j são respectivamente 0 <= i < N  e  0 <= j < M, respectivamente. Sendo assim, você deve alterar o código do VPL de forma que todos os métodos que recebem as coordenadas [i, j] de células como parâmetro lancem uma exceção do tipo InvalidCellException, caso estas coordenadas sejam inválidas:

struct GameOfLife::InvalidCellException {
    int line;   // Linha da célula inválida.
    int column;   // Coluna da célula inválida.
}; 

(iv) a função main() recebe pela entrada padrão os seguintes dados:

<número de iterações a serem executadas>

<número de linhas na matriz>  <número de colunas na matriz>

<linha da 1a. célula via>  <coluna da 1a. célula via>

<linha da 2a. célula via>  <coluna da 2a. célula via>

<linha da 3a. célula via>  <coluna da 3a. célula via>

...

Altere a função main() de forma que quando as coordenadas de um célula forem inválidas, a seguinte mensagem de erro seja exibida para tratar esta exceção:

catch (GameOfLife::InvalidCellException e) {
    cerr << "As coordenadas [" << e.line << ", " << e.column << "] não são de uma célula válida" << endl;
    return 1;

} 
*/
#include <iostream>
#include <chrono>
#include <thread>

#include "game_of_life.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using namespace std::chrono_literals;

void Print(const GameOfLife& g) {
  cout << "  "; 
  for (int i = 0; i < g.columns(); ++i) {
    cout << "- "; 
  }
  cout << endl;
  for (int i = 0; i < g.lines(); ++i) {
    cout << "| ";
    for (int j = 0; j < g.columns(); ++j) {
      if (g.alive(i, j)) {
        cout << "* "; 
      } else {
        cout << "  ";
      }
    }
    cout << "|" << endl;
  }
  cout << "  "; 
  for (int i = 0; i < g.columns(); ++i) {
    cout << "- "; 
  }
  cout << endl;
}

#include <memory>
#include <string>

using namespace std;

int main() {
  // Lê os dados.
  int iterations;
  cin >> iterations;
  int line, column;
  int line_total, column_total;
  cin >> line >> column;
  line_total = line;
  column_total = column;
  GameOfLife g(line, column);
  while (cin >> line) {
    cin >> column;
    try {
      if (line < 0 || line >= line_total) throw GameOfLife::InvalidCellException{line , column};
      if (column < 0 || column >= column_total) throw GameOfLife::InvalidCellException{line , column};
      g.Enliven(line, column);
    } catch (GameOfLife::InvalidCellException e) {
      cerr << "As coordenadas [" << e.line << ", " << e.column << "] não são de uma célula válida" << endl;
      return 1;
    }
    
  }
  Print(g);
    
  // Executa o número esperado de iterações.
  while (iterations > 0) {
    g++;
    Print(g);
    std::this_thread::sleep_for(10ms);
    iterations--;
  }
}