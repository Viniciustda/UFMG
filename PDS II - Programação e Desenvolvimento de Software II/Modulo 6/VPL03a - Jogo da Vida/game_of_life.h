#ifndef GAME_OF_LIFE_H_
#define GAME_OF_LIFE_H_

#include <vector>

using std::vector;

// Simula a execução de cada iteração do Jogo da Vida de Cownaway. 
class GameOfLife {
 public:
  struct InvalidCellException {
    int line;  // Linha da célula inválida.|
    int column;  // Coluna da célula inválida.|
  }; 

  // Cria um jogo da vida com determinado número de linhas e colunas.
  GameOfLife(int lines, int columns);

  // Retorna o número de linhas.
  int lines() const;

  // Retorna o número de colunas.
  int columns() const;

  // Muda o estado da cécula [i, j] para viva.
  void Enliven(int i, int j);
  
  // Muda o estado da cécula [i, j] para morta.
  void Kill(int i, int j);

  // Retorna #true se a célula [i, j] está viva, ou #false caso contrário.
  bool alive(int i, int j) const;

  // Executa a próxima iteração do jogo da vida.
  void operator++(int);

 private:
 
  // Retorna o número de vizinhos da célula [i, j]
  int neighbors(int i, int j) const;

  // Retorna #true se a célula [i, j] estará viva na próxima iteração,
  // ou #false caso contrário.
  bool lives(int i, int j) const;

  vector<vector<bool>> torus_;   
};

#endif  // GAME_OF_LIFE_H_