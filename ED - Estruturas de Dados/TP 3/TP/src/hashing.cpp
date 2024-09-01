#include "hashing.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int Hash(string const& s, int m) {
    const int p = 31;
    int hash_value = 0;
    int p_pow = 1;
    for (char c : s) {
        hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
    return hash_value;
}
