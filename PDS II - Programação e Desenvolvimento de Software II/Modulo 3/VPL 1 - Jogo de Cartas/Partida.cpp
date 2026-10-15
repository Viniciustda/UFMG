/*
#include "Partida.hpp"
#include "Jogador.hpp"

Partida::Partida(int num_jogadores) {
    this->_num_jogadores = num_jogadores;
}

Partida::~Partida() {
    cout << "~Partida" << endl;
}

void Partida::addJogadorCarta(string nomeJogador, int numero_pontos_carta, string naipe) {
    Carta *carta;
    carta->setNumeroPontosPadrao(numero_pontos_carta);
    carta->setNaipe(naipe);

    Jogador jogador;
    jogador.setNomeJogador(nomeJogador);
    jogador.adicionaCarta(*carta);

    this->_jogadores.push_back(jogador);
}

	
int Partida::getNumJogadores() {
    return this->_num_jogadores;
}

int Partida::getNumAtualJogadores() {
    return this->_atualSize;
}

void Partida::setJogadores(vector<Jogador> jogadores) {
    this->_jogadores = jogadores;
}

vector<Jogador> Partida::getJogadores() {
    return this->_jogadores;
}

Jogador Partida::getCampeao() {
    this->_jogadores = getJogadoresOrdenados();

    return _jogadores[_num_jogadores-1];
}

vector<Jogador> Partida::getJogadoresOrdenados() {

    int min = 0;
    Jogador aux;
    for (int i = 0; i < _num_jogadores-1; i++) {
        min = i;
        for (int j = i+1; j < _num_jogadores; j++) {
            if (this->_jogadores[j].calcularPontuacao() < this->_jogadores[min].calcularPontuacao()) {
                min = j;
            }
        }
        if (this->_jogadores[i].calcularPontuacao() > this->_jogadores[min].calcularPontuacao()) {
            aux = this->_jogadores[i];
            this->_jogadores[i] = this->_jogadores[min];
            this->_jogadores[min] = aux;
        }
            
    }

    return _jogadores;

}

void Partida::imprimeJogadoresOrdenados() {
    this->_jogadores = getJogadoresOrdenados();
        
    for (int i = 0; i < _num_jogadores; i++) {
        cout << this->_jogadores[i].getNomeJogador() << ' ' << this->_jogadores[i].calcularPontuacao() << endl;
        }
        
}
*/

#include "Partida.hpp"
#include "Jogador.hpp"

Partida::Partida(int num_jogadores) {
    this->_num_jogadores = num_jogadores;
    this->_jogadores = new Jogador[num_jogadores];
}

Partida::~Partida() {
    delete _jogadores;
    cout << "~Partida" << endl;
}

void Partida::addJogadorCarta(string nomeJogador, int numero_pontos_carta, string naipe) {

    Carta carta(numero_pontos_carta, naipe);

    Jogador novo_jogador;

    bool existe_jogador = false;

    for (int i = 0; i < _atualSize; i++) {
        if (this->_jogadores[i].getNomeJogador() == nomeJogador) {
            this->_jogadores[i].adicionaCarta(carta);
            existe_jogador = true;
        }
    }
    if (existe_jogador == false) {
        this->_jogadores[_atualSize].setNomeJogador(nomeJogador);
        this->_jogadores[_atualSize].adicionaCarta(carta);
        this->_atualSize ++;
    }
}
	
int Partida::getNumJogadores() {
    return this->_num_jogadores;
}

int Partida::getNumAtualJogadores() {
    return this->_atualSize;
}

void Partida::setJogadores(Jogador* jogadores) {
    this->_jogadores = jogadores;
}

Jogador* Partida::getJogadores() {
    return this->_jogadores;
}

Jogador Partida::getCampeao() {
    this->_jogadores = getJogadoresOrdenados();

    return this->_jogadores[_num_jogadores-1];
}

Jogador* Partida::getJogadoresOrdenados() {

    int min = 0;
    Jogador aux;
    for (int i = 0; i < this->_num_jogadores-1; i++) {
        min = i;
        for (int j = i+1; j < this->_num_jogadores; j++) {
            if (this->_jogadores[j].calcularPontuacao() < this->_jogadores[min].calcularPontuacao()) {
                min = j;
            }
        }
        if (this->_jogadores[i].calcularPontuacao() > this->_jogadores[min].calcularPontuacao()) {
            aux = this->_jogadores[i];
            this->_jogadores[i] = this->_jogadores[min];
            this->_jogadores[min] = aux;
        }
            
    }

    return this->_jogadores;

}

void Partida::imprimeJogadoresOrdenados() {
    this->_jogadores = getJogadoresOrdenados();
        
    for (int i = 0; i < this->_num_jogadores; i++) {
        cout << this->_jogadores[i].getNomeJogador() << ' ' << this->_jogadores[i].calcularPontuacao() << endl;
    }
        
}