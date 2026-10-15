#include <iostream>

#include "Point.h"

std::ostream & operator << (std::ostream &out, const Point &p) {
  // TODO: implement this operator.
  return out << '(' << p.getX() << ',' << p.getY() << ')';
}

std::istream & operator >> (std::istream &in,  Point &p) {
  // TODO: implement this operator.
  in >> p.x >> p.y;
  return in;
}

Point Point::operator + (const Point &p) {
  // TODO: implement this operator.
  double new_x = x + p.getX();
  double new_y = y + p.getY();
  x = new_x;
  y = new_y;
  return *this;
}

Point& Point::operator += (const Point &p) {
  // TODO: implement this operator.
  double new_x = x + p.getX();
  double new_y = y + p.getY();
  x = new_x;
  y = new_y;
  return *this;
}