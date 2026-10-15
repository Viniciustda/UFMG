#ifndef PONTO_H
#define PONTO_H
#include <iostream>
#include <string>


/**
 * \Interface que representa cada ponto, independente de sua dimensão.
 */
class Ponto {
  public:
    /**
     * \Representação de string do ponto.
     * \retorna uma string que representa o ponto, como uma tupla de n-dimensões.
     */
    virtual std::string to_string() const = 0;
    /**
     * \Computa a distância entre o Ponto armazenado no objeto e o Ponto passado por parâmetro (se possuem a mesma dimensão)
     */
    virtual double distancia(Ponto* p) const = 0;
	/**
     * Retorna uma inteiro que representa a dimensão do ponto.
     */
	virtual int getDimensao() const = 0;
    /**
     * \o destruidor virtual (ou então o compilador irá criar um para você).
     */
    virtual ~Ponto() {};
};

#endif