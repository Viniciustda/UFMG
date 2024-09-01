int ddd(unsigned int number) {
    return number / 100000000;
}

int soma1SePar(unsigned int number) {
    return number | (unsigned int)1;
}

int parOuImpar(unsigned int number) {
    return !(number &(unsigned int)1);
}