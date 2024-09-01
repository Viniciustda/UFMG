#include "queue.h"

struct Node {
  	int key;
  	Node* next;
};

Queue::Queue() {
	this->tamanho = 0;
	this->back_ = nullptr;
	this->front_ = nullptr;
}

void Queue::push(int k) {
	Node* novo_no = new Node;
	novo_no->key = k;


	if (this->tamanho == 0) {
		this->front_ = novo_no;
		this->back_ = novo_no;
	} 
	else if(this->tamanho == 1) {
		this->back_ = novo_no;
		this->front_->next = this->back_;
	} 
	else {
		Node* aux = this->back_;
		this->back_ = novo_no;
		aux->next = this->back_;
	}
	this->tamanho++;
}

void Queue::pop() {
	if (this->tamanho == 0) throw EmptyException();
	else {
		Node* node = this->front_;
		this->front_ = node->next;
		this->tamanho--;
		delete node;
	}
}

int Queue::front() const {
	if (this->tamanho == 0) throw EmptyException();
	else {
		return this->front_->key;
	}
}

int Queue::back() const {
	if (this->tamanho == 0) throw EmptyException();
	else {
		return this->back_->key;
	}
}

int Queue::count() const {
	return this->tamanho;
}