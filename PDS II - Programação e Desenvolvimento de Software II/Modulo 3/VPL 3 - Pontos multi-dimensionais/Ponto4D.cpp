#include "Ponto4D.hpp"

std::string Ponto4D::to_string() const {
    if(this->getDimensao() == 4) {
        std::string x = std::to_string(this->_x);
        std::string y = std::to_string(this->_y);
        std::string z = std::to_string(this->_z);
        std::string w = std::to_string(this->_w);
        std::string str = '(' + x + ',' + y + ',' + z + ',' + w + ')';
        return str;
    }
}

double Ponto4D::distancia(Ponto* p) const {
    if(p->getDimensao() == 4) {
		Ponto4D* p4d = dynamic_cast<Ponto4D*>(p);
        return sqrt(pow(this->_x - p4d->_x, 2) + pow(this->_y - p4d->_y, 2) + pow(this->_z - p4d->_z, 2) + pow(this->_w - p4d->_w, 2));
    }
}