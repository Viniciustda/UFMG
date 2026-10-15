#include "Ponto2D.hpp"

std::string Ponto2D::to_string() const {
    if(this->getDimensao() == 2) {
        std::string x = std::to_string(this->_x);
        std::string y = std::to_string(this->_y);
        std::string str = '(' + x + ',' + y + ')';
        return str;
    }
}

double Ponto2D::distancia(Ponto* p) const {
    if(p->getDimensao() == 2) {
		Ponto2D* p2d = dynamic_cast<Ponto2D*>(p);
        return sqrt(pow(this->_x - p2d->_x, 2) + pow(this->_y - p2d->_y, 2));
    }
}

void Ponto2D::converteParaPolar() {
    double r;
    double theta;

    r = sqrt(pow(this->_x, 2) + pow(this->_y, 2));
    theta = atan(this->_y / this->_x);

    this->_x = r;
    this->_y = theta;
}