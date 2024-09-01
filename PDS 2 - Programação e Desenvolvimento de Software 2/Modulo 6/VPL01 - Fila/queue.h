#ifndef QUEUE_H
#define QUEUE_H

struct Node;

// Fila de números inteiros.
class Queue {

public:
    // Representa uma exceção que ocorre quando a fila está vazia.
    // Ela não tem estado, já que toda fila vazia é igual.
    // Consequetemente, sua implementação é vazia.
    class EmptyException {
    };

    // Constrói uma fila vazia.
    Queue();

    // Insere um elemento no fim da fila.
    void push(int k);

    // Retorna o elemento no início da fila.
    // Lança uma exceção do tipo EmptyException caso a fila esteja vazia.
    int front() const;

    // Retorna o elemento no fim da fila.
    // Lança uma exceção do tipo EmptyException caso a fila esteja vazia.
    int back() const;

    // Remove o elemento no início da fila.
    // Lança uma exceção do tipo EmptyException caso a fila esteja vazia.
    void pop();

    // Retorna o número de elementos na fila.
    int count() const;

private:
    Node *front_;
    Node *back_;
    int tamanho;
};

#endif // QUEUE_H