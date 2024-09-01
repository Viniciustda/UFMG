#include <iostream>
#include <string>

using namespace std;

#define TAM 65

class Camiseta {
   
    public:
        string aluno;
        string tamanho;
        string cor;
};

int main() {
    
    Camiseta camisetas[TAM];
    string entrada;
    string aux;
    int n = 0;
    int indice = 0;

    getline(cin, entrada);
    n = stoi(entrada);

    while (n) {

        for (int i = 0; i < n; i++) {
            
            getline(cin, entrada);
            camisetas[i].aluno = entrada;

            getline(cin, entrada);
            aux = entrada;
            indice = entrada.find_first_of(' ',0);

            entrada.erase(indice, entrada.length());
            camisetas[i].cor = entrada;

            aux.erase(0, indice);
            camisetas[i].tamanho = aux;
            camisetas[i].tamanho += " ";

        }

        for (indice = 0; indice < n; ++indice) {

            int indiceMenor = indice;

            for (int indiceSeguinte = indice+1; indiceSeguinte < n; ++indiceSeguinte) {

                if (camisetas[indiceSeguinte].cor < camisetas[indiceMenor].cor) 
                    indiceMenor = indiceSeguinte;

                if (camisetas[indiceSeguinte].cor == camisetas[indiceMenor].cor && camisetas[indiceSeguinte].tamanho > camisetas[indiceMenor].tamanho) 
                    indiceMenor = indiceSeguinte;
        
                if (camisetas[indiceSeguinte].cor == camisetas[indiceMenor].cor && camisetas[indiceSeguinte].tamanho == camisetas[indiceMenor].tamanho && camisetas[indiceSeguinte].aluno < camisetas[indiceMenor].aluno) 
                    indiceMenor = indiceSeguinte;
                
            }

            Camiseta aux = camisetas[indice];
            camisetas[indice] = camisetas[indiceMenor];
            camisetas[indiceMenor] = aux;

        }

        for (int i = 0; i < n; i++)
            cout << camisetas[i].cor << camisetas[i].tamanho << camisetas[i].aluno << endl;
          
        
        getline(cin, entrada);
        n = stoi(entrada);

        if (n) cout << endl;
        
    }

    return 0;
}

/*

1258

O professor Rolien organizou junto às suas turmas de Ciência da Computação a confecção de uma camiseta polo que fosse ao mesmo tempo bonita e barata. Após algumas conversas, ficou decidido com os alunos que seriam feitas somente camisetas da cor preta, o que facilitaria a confecção. Os alunos poderiam escolher entre o logo do curso e os detalhes em branco ou vermelho. Assim sendo, Rolien precisa de sua ajuda para organizar as listas de quem quer a camiseta em cada uma das turmas, relacionando estas camisetas pela cor do logo do curso, tamanho (P, M ou G) e por último pelo nome.

Entrada
A entrada contém vários casos de teste. Cada caso de teste inicia com um valor N, (1 ≤ N ≤ 60) inteiro e positivo, que indica a quantidade de camisetas a serem feitas para aquela turma. As próximas N*2 linhas contém informações de cada uma das camisetas (serão duas linhas de informação para cada camiseta). A primeira linha irá conter o nome do estudante e a segunda linha irá conter a cor do logo da camiseta ("branco" ou "vermelho") seguido por um espaço e pelo tamanho da camiseta "P" "M" ou "G". A entrada termina quando o valor de N for igual a zero (0) e esta valor não deverá ser processado.

Saída
Para cada caso de entrada deverão ser impressas as informações ordenadas pela cor dos detalhes em ordem ascendente, seguido pelos tamanhos em ordem descendente e por último por ordem ascendente de nome, conforme o exemplo abaixo.

Obs.: Deverá ser impressa uma linha em branco entre dois casos de teste.

*/