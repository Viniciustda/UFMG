#include "Ponto3D.hpp"

std::string Ponto3D::to_string() const {
    if(this->getDimensao() == 3) {
        std::string x = std::to_string(this->_x);
        std::string y = std::to_string(this->_y);
        std::string z = std::to_string(this->_z);
        std::string str = '(' + x + ',' + y + ',' + z + ')';
        return str;
    }
}

double Ponto3D::distancia(Ponto* p) const {
    if(p->getDimensao() == 3) {
		Ponto3D* p3d = dynamic_cast<Ponto3D*>(p);
        return sqrt(pow(this->_x - p3d->_x, 2) + pow(this->_y - p3d->_y, 2) + pow(this->_z - p3d->_z, 2));
    }
}