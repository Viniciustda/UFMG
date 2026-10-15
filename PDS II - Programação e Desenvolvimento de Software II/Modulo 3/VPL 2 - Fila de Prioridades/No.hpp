#ifndef NO
#define NO

#include <iostream>

using namespace std;

class No {
  private:
    int _dado;
    No * _proximo;
	int _prioridade;
  public:
    No(int prioridade, int dado, No* proximo);
    
	/** Seta o próximo nó.
     */
    void setProximo(No *next);
    /** Retorna o próximo nó.
     */
    No* getProximo();
	
	/** Retorna o dado armazenado no nó. 
       @retorna um inteiro.
     */
    int getDado();
	/** Seta o valor do dado que será armazenado no nó. 
     */
	void setDado(int dado);
	
	/** Retorna o valor da prioridade do nó. 
     */
	int getPrioridade();
	/** Seta o valor da prioridade do nó. 
     */
	void setPrioridade(int prioridade);
};

#endif