#include <iostream>
#include <string>

#define TAM 55

using namespace std;

class Vertice {
    
    public:
        Vertice();

        int id;
        int grau;
        bool parenteses_aberto;
        bool ativo;
        bool tem_pai;
        Vertice *pai;

};

Vertice::Vertice() {
    this->id = 0;
    this->grau = 0;
    this->parenteses_aberto = false;
    this->ativo = true;
    this->tem_pai = false;
    this->pai = nullptr;
}

class Arvore {
    
    public:
        Arvore();

        int tamanho;
        Vertice nos[TAM];

};

Arvore::Arvore() {
    this->tamanho = 0;
}

int main() {
    
    string entrada;

    while(getline(cin, entrada)) {

        Arvore arvore;
        string aux;
        int indice = 0;

        while (indice < entrada.length()-1) {

            if (entrada[indice] == ')' && entrada[indice+1] == ')') {
                for (int i = arvore.tamanho; i >= 0; i--) {
                    if (arvore.nos[i].parenteses_aberto == true) {
                        arvore.nos[i].parenteses_aberto = false;
                        break;
                    }
                } 
            }
            
        
            if (entrada[indice] != '(' && entrada[indice] != ')' && entrada[indice] != ' ') {
                if (entrada[indice+1] != '(' && entrada[indice+1] != ')' && entrada[indice+1] != ' ') {
            
                    aux = entrada;
                    aux.erase(0,indice);
                    aux.erase(2,aux.length());

                    arvore.nos[arvore.tamanho].id = stoi(aux);
            
                    indice++;

                }
                else if (entrada[indice+1] == '(' || entrada[indice+1] == ')' || entrada[indice+1] == ' ') {
                    aux = entrada;
                    aux.erase(0,indice);
                    aux.erase(1,aux.length());

                    arvore.nos[arvore.tamanho].id = stoi(aux);
                    
                }


                if (arvore.tamanho > 0) {
                    for (int i = arvore.tamanho-1; i >= 0; i--) {
                        if (arvore.nos[i].parenteses_aberto == true) {
                            arvore.nos[arvore.tamanho].pai = &arvore.nos[i];
                            arvore.nos[arvore.tamanho].tem_pai = true;
                            arvore.nos[i].grau++; 
                            break;
                        }
                    }                    
                }
                    

                if (entrada[indice+1] == ' ') {
                    arvore.nos[arvore.tamanho].parenteses_aberto = true;
                }

                arvore.tamanho++;

            }
            
            indice++;
        
        }

        if (arvore.tamanho > 1) {
            string saida;
            Vertice *folha;
            int indice_aux = 0;

            indice = 0;
            while (arvore.nos[indice].grau != 0) {
                indice++;
            }
            folha = &arvore.nos[indice];
            
            indice = arvore.tamanho-2;
            while (indice--) {

                for (int i = 0; i < arvore.tamanho; i++) {
                    if (arvore.nos[i].ativo == true && (arvore.nos[i].grau == 0 || (arvore.nos[i].grau == 1 && arvore.nos[i].tem_pai == false)) && arvore.nos[i].id < folha->id) {
                        folha = &arvore.nos[i];
                    }
                }

                if (folha->tem_pai) {
                    saida += to_string(folha->pai->id);
                    saida += " ";

                    folha->ativo = false;
                    folha->pai->grau--;
                } 
                else {
                    folha->ativo = false;
                    folha->grau--;
                    
                    indice_aux = 0;
                    while (!arvore.nos[indice_aux].tem_pai || arvore.nos[indice_aux].pai->id != folha->id || !arvore.nos[indice_aux].ativo) indice_aux++;
                    folha = &arvore.nos[indice_aux];

                    saida += to_string(folha->id);
                    saida += " ";
                    
                    folha->tem_pai = false;
                }

                for (int i = 0; i < arvore.tamanho; i++) {
                    if (arvore.nos[i].ativo) 
                        folha = &arvore.nos[i];
                }
                
                
            }

            for (int i = 0; i < arvore.tamanho; i++) {
                if (arvore.nos[i].ativo && arvore.nos[i].id > folha->id) folha = &arvore.nos[i];
            }

            saida += to_string(folha->id);
            
            cout << saida << endl;
        }
        else {
            cout << endl;
        }
        
        
        
    }


    return 0;
}

/*

1735

Uma árvore (isto é, um grafo conexo sem ciclos) com os vértices numerados por números inteiros 1, 2, ..., n é dado. O código "Prufer" de tal estrutura é construído como da seguinte forma: a folha (um vértice que é incidente a uma única aresta) com o menor número é tomado. Esta folha, juntamente com a sua aresta incidente é removida do grafo, enquanto o número do vértice que era adjacente à folha é anotado. No grafo obtido, este procedimento se repete, até que haja apenas um vértice restante (que, por sinal, sempre tem o número n). A sequência de escrita com n-1 números, é chamado o código Prufer da árvore.

Sua tarefa é, dada uma árvore, para calcular o seu código Prufer. A árvore é indicada por uma palavra do idioma especificado pela seguinte gramática:

T ::= "(" N S ")"

S ::= " " T S

| vazio

N ::= número

Ou seja, as árvores têm parênteses em torno deles, e um número indicando o identificador do vértice raiz, seguido por arbitrariamente muitas (talvez nenhuma) subárvores separadas por um único caractere de espaço. Como exemplo, dê uma olhada na árvore na figura abaixo que é indicado na primeira linha da entrada de amostra.

Observe que, de acordo com a definição dada acima, a raiz de uma árvore pode ser uma folha também. É só para facilitar a denotação que designa algum vértice para ser a raiz. Normalmente, o que estamos lidando aqui com é chamada de "árvore não enraizada".



Entrada
A entrada contém vários casos de teste. Cada caso de teste especifica uma árvore, como descrito acima em uma linha do arquivo de entrada. A entrada é terminada por EOF. Você pode assumir que 1 ≤ n ≤ 50.

Saída
Para cada caso de teste imprima uma única linha que contém o código Prufer da árvore especificada. Separe os números por um único espaço. Não imprima espaços no final da linha.

*/