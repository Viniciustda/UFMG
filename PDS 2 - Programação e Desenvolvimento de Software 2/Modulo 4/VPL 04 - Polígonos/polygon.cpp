/**
 * Todas as tarefas deste exercicio devem ser feitas sobre esse arquivo.
 * Os metodos e operacoes marcados com a tag "TODO" devem ser editados.
 */

#include <iostream>
#include "polygon.h"

std::ostream& operator << (std::ostream &out, const Polygon &poly) {
  for (const Point& p: poly.limits) {
    out << " " << p;
  }
  return out;
}

bool operator == (const Polygon &lhs, const Polygon &rhs) {
  // TODO: implement this method.
  std::vector<Point> limits_lhs = (const std::vector<Point>&)lhs;
  std::vector<Point> limits_rhs = (const std::vector<Point>&)rhs;
  
  bool igual = false;
  
  int tamanho_lhs = limits_lhs.size();
  int tamanho_rhs = limits_rhs.size();

  if (tamanho_lhs == 2 && tamanho_rhs == 2) {
    
    Point pt3_lhs (limits_lhs[1].x, limits_lhs[0].y);
    Point pt4_lhs (limits_lhs[0].x, limits_lhs[1].y);

    limits_lhs.push_back(pt3_lhs);
    limits_lhs.push_back(pt4_lhs);

    Point pt3_rhs (limits_rhs[1].x, limits_rhs[0].y);
    Point pt4_rhs (limits_rhs[0].x, limits_rhs[1].y);

    limits_rhs.push_back(pt3_rhs);
    limits_rhs.push_back(pt4_rhs);

    tamanho_lhs = limits_lhs.size();
    tamanho_rhs = limits_rhs.size();
  }
  
  if (tamanho_lhs == tamanho_rhs) {
    int aux = 0;
    int aux2 = 0;

    for (int i = 0; i < tamanho_lhs; i++) {
      for (int j = 0; j < tamanho_rhs; j++) {
        if ((limits_lhs[i].x == limits_rhs[j].x) && (limits_lhs[i].y == limits_rhs[j].y)) {
          aux++;
          break;
        }
      }
    }

    for (int i = 0; i < tamanho_rhs; i++) {
      for (int j = 0; j < tamanho_lhs; j++) {
        if ((limits_rhs[i].x == limits_lhs[j].x) && (limits_rhs[i].y == limits_lhs[j].y)) {
          aux2++;
          break;
        }
      }
    }
    
    
    if (aux == aux2 && aux == tamanho_rhs) {
      igual = true;
    }
  }

  bool zerado_lhs = false;
  int aux_zerado = 0;
  for (int i = 0; i < tamanho_lhs; i++) {
    if (limits_lhs[i].x == 0 && limits_lhs[i].y == 0) {
      aux_zerado++;
    }
  }
  if (aux_zerado == tamanho_lhs) {
    zerado_lhs = true;
  }

  bool zerado_rhs = false;
  aux_zerado = 0;
  for (int i = 0; i < tamanho_rhs; i++) {
    if (limits_rhs[i].x == 0 && limits_rhs[i].y == 0) {
      aux_zerado++;
    }
  }
  if (aux_zerado == tamanho_rhs) {
    zerado_rhs = true;
  }

  if (zerado_lhs && zerado_rhs) {
    igual = true;
  }
  
  if (tamanho_lhs == 1 && tamanho_rhs == 1) {
    if ((limits_lhs[0].x == limits_rhs[0].x) && (limits_lhs[0].y == limits_rhs[0].y)) {
      igual = true;
    } else {
      igual = false;
    }
    
  }
  
  return igual;
}

bool Point::contains(const Point& p) const {
  return p.x == this->x && p.y == this->y;
}

std::ostream& operator << (std::ostream &out, const Point &p) {
  out << "(" << p.x << ", " << p.y << ")";
  return out;
}

bool RightRectangle::contains(const Point& p) const {
  // TODO: implement this method.
  //std::vector<Point> poligono = (const std::vector<Point>&)p;
  bool contem_pt = false;
  bool contem_x = false;
  bool contem_y = false;

  if(this->limits[0].x < this->limits[1].x) {
    if(p.x >= this->limits[0].x && p.x <= this->limits[1].x) {
      contem_x = true;
    }
  }
  if(this->limits[0].x > this->limits[1].x) {
    if(p.x <= this->limits[0].x && p.x >= this->limits[1].x) {
      contem_x = true;
    }
  }

  if(this->limits[0].y < this->limits[1].y) {
    if(p.y >= this->limits[0].y && p.y <= this->limits[1].y) {
      contem_y = true;
    }
  }
  if(this->limits[0].y > this->limits[1].y) {
    if(p.y <= this->limits[0].y && p.y >= this->limits[1].y) {
      contem_y = true;
    }
  }
  
  if(contem_x == true && contem_y == true) {
    contem_pt = true;
  }
  
  return contem_pt;
}

Point::Point(int xx, int yy): x(xx), y(yy) {
  // TODO: implement this method.
  this->limits.push_back(*this);
}

RightRectangle::RightRectangle(int x0, int y0, int x1, int y1) {
  // TODO: implement this method.
  Point a (x0, y0);
  Point b (x1, y1);

  this->limits.push_back(a);
  this->limits.push_back(b);
}