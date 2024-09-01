#ifndef GET_MAX_H
#define GET_MAX_H

#include <iostream>

/**
 * \brief This class represents a bit set, that is, a set represented as a map
 * of bits. The set has capacity to store 32 elements. For instance, the set
 * <||||----------------------------> is the binary representation of the number
 * 15, and it means that the elements 1, 2, 3 and 4 are in the set. Similarly,
 * the set <--||----------------------------> is the binary representation of
 * the number 12, and it means that the elements 3 and 4 are in the set.
 */
struct BitSet {
  BitSet(unsigned value): _set(value) {}
  const unsigned _set;
  friend std::ostream & operator<<(std::ostream& os, const BitSet& i) {
    const int limit = sizeof(unsigned) * 8;
    os << '<';
    for (int aux = 0; aux < limit; aux++) {
      unsigned mask = 1 << aux;
      if (i._set & mask) {
        os << "|";
      } else {
        os << "-";
      }
    }
    os << '>';
    return os;
  }
  /**
   * \brief compares two bit sets. A bit set b1 is greater than or equal to
   * another bit set b2 if b1 contains all the bits in b2. For instance, the
   * bit set represented by 15 is greater than the bit set represented by 12.
   * However, the bit set 6 is not greater than the bit set 3, because the
   * former does not contain the first element, which is in the latter.
   * \return true if bit_set1 is greater than bit_set2
   */
  // TODO: implement this operator.
  bool operator>= (const BitSet& il) {
    unsigned inicio1 = 0;
    unsigned fim1 = 0;
    unsigned inicio2 = 0;
    unsigned fim2 = 0;
    const int limit = sizeof(unsigned) * 8;
    for (int aux = 0; aux < limit; aux++) {
      unsigned mask = 1 << aux;
      if (_set & mask) {
        if (inicio1 == 0) {
          inicio1 = mask;
        }
        fim1 = mask;
      }
    }
    for (int aux = 0; aux < limit; aux++) {
      unsigned mask = 1 << aux;
      if (il._set & mask) {
        if (inicio2 == 0) {
          inicio2 = mask;
        }
        fim2 = mask;
      }
    }
    return inicio1 <= inicio2 && fim1 >= fim2;
  }
};

/**
 * \brief The representation of an integer interval. An integer interval has a
 * lower bound (a left side), and an upper bound (a right side). For instance,
 * the interval (2, 6) contains all the elements from 2 to 6, including these
 * two.
 */
struct Interval {
  Interval(int left, int right): _l(left), _r(right) {}
  const int _l;
  const int _r;
  friend std::ostream & operator<<(std::ostream& os, const Interval& i) {
    os << '(' << i._l << ", " << i._r << ')';
    return os;
  }
  /**
   * \brief compares two intervals to see if the first is greater than or equal
   * the second. An interval (a1, a2) is greater than or equal another interval
   * (b1, b2) if a1 <= b1 and a2 >= b2.
   * \return true if interval1 is greater than or equal interval2
   */
  // TODO: implement this operator.
  bool operator>= (const Interval& il) {
    return _l <= il._l && _r >= il._r;
  }
};

/**
 * \brief Returns the greatest of two elements, or a default, if neither of
 * them is the greatest.
 */
template <class T>
T GetMaxDefault (T a, T b, T dflt) {
  // TODO: implement this generic function.
  if ((a >= b) && !(b >= a)) {
    return a;
  } else if ((b >= a) && !(a >= b)) {
    return b;
  } else if ((a >= b) && (b >= a)) {
    return a;
  } else {
    return dflt;
  }  
}

#endif