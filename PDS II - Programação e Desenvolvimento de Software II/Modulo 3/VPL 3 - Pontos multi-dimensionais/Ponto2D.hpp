#ifndef PONTO_2D
#define PONTO_2D

#include "Ponto.hpp"
#include <cmath>
/**
 * \Um ponto bidimensional.
 */
class Ponto2D: public Ponto {
  public:
    Ponto2D(double x, double y): _x(x), _y(y) {}
    virtual double getY() const { return _y; }
	virtual double getX() const { return _x; }
	
    virtual std::string to_string() const override;

    /**
	 * A distância entre pontos de duas dimensões é calculada da seguinte forma
     * que é: sqrt((x1 - x2)^2 + (y1 - y2)^2).
	 * Onde x1 é a coordenada x do primeiro ponto, e x2 é a coordenada x do segundo ponto
	 * E y1 é a coordenada y do primeiro ponto, e y2 é a coordenada y do segundo ponto
     */
    virtual double distancia(Ponto* p) const override;
	virtual int getDimensao() const override {return 2;};
	
	/** 
	 * Converte as coordenadas _x e _y para uma representação polar de duas coordenadas ( r e theta )
	 * O r é calculado da seguinte forma sqrt((x)^2 + (y)^2).
	 * O theta é calculado da seguinte forma atan(y/x)
	 * Após a conversão o método atualiza x como sendo r, e y como sendo theta
	 */
	void converteParaPolar();
	
  protected:
	double _x; ///< A primeira coordenada do ponto.
    double _y; ///< A segunda coordenada do ponto.

};

#endif
