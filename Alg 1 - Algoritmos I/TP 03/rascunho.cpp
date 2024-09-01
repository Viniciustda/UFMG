/*
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int num_itens, capacidade;
    cin >> num_itens >> capacidade;

    struct Item { int peso, valor; string nome; }; 
    vector<Item> itens(num_itens);

    for (auto& [peso, valor, nome] : itens) {
        cin >> nome >> peso >> valor;
    }

    vector<vector<int>> dp(1e4+1, vector<int>(1e4+1));
    vector<vector<int>> nxt(1e4+1, vector<int>(1e4+1));

    for (int i = num_itens; i >= 0; i--) {
        for (int j = 0; j <= 1e4; j++) {
            if (i == num_itens) {
                dp[i][j] = 0;
                continue;
            }

            nxt[i][j] = j;
            int melhor_valor = dp[i+1][j]; // pular o item

            if (j >= itens[i].peso) { // pegar o item
                int novo_valor = dp[i+1][j - itens[i].peso] + itens[i].valor;
                if (novo_valor > melhor_valor) {
                    melhor_valor = novo_valor;
                    nxt[i][j] = j - itens[i].peso;
                }
            }

            dp[i][j] = melhor_valor;
        }
    }

    for (int i = 0; i < num_itens; capacidade = nxt[i][capacidade], i++) {
        if (nxt[i][capacidade] != capacidade) {
            cout << itens[i].nome << "\n";
        }
    }

    return 0;
}
*/

/*
#include <iostream>
#include <vector>

using namespace std;

#define INF 0x7FFFFFFFFFFFFFFF

struct Secao {
    int bonificacao;
    int tempo_travessia;
    vector <int> manobras;
};

struct Manobra {
    int nome;
    int pontuacao;
    int tempo_manobra;
};

// Descobre a travessia mais radical
void TravessiaMaisRadical (int num_secoes, int num_manobras, vector <Secao> secoes, vector <Manobra> manobras) {

    // Declara variaveis
    int pontuacao_max = 0;
    int capacidade = 0;
    vector<vector<int>> dp;
    vector<vector<int>> nxt;

    // Problema das machilas
    for (int i = 1; i <= num_secoes; i++) {

        capacidade = secoes[i].tempo_travessia;
    
        dp.resize(num_manobras+1, vector<int>(capacidade+1, 0));
        nxt.resize(num_manobras+1, vector<int>(capacidade+1, 0));

        for (int j = num_manobras; j >= 0; j--) {
            for (int k = 0; k <= capacidade; k++) {
                if (j == num_manobras) {
                    dp[j][k] = 0;
                    continue;
                }

                nxt[j][k] = k;
                int melhor_valor = dp[j+1][k];

                if (k >= manobras[j].tempo_manobra) {
                    int novo_valor = dp[j+1][k - manobras[j].tempo_manobra] + manobras[j].pontuacao;
                    if (novo_valor > melhor_valor) {
                        melhor_valor = novo_valor;
                        nxt[j][k] = k - manobras[j].tempo_manobra;
                    }
                }

                dp[j][k] = melhor_valor;
            }
        }

        pontuacao_max += dp[0][capacidade];

        for (int j = 0; j < num_manobras; capacidade = nxt[j][capacidade], j++) {
            if (nxt[j][capacidade] != capacidade) {
                secoes[i].manobras.push_back(manobras[j].nome);
            }
        }

    }

    // Imprime pontuacao maxima
    cout << pontuacao_max << endl;

    // Imprime numero de manobras por secao e quais manobras executadas na secao
    for (int i = 1; i <= num_secoes; i++) {
        cout << secoes[i].manobras.size();
        for (int j = 0; j < secoes[i].manobras.size(); j++)
            cout << " " << secoes[i].manobras[j];
        cout << endl;
    }
    
}

int main(int argc, char const *argv[]) {

    // Declara variaaveis
    int num_secoes = 0, num_manobras = 0;
    vector <Secao> secoes;
    vector <Manobra> manobras;

    // Recebe numero de secoes e numero de manobras
    cin >> num_secoes >> num_manobras;

    // Atualiza tamanho dos vetores de secoes e manobras
    secoes.resize(num_secoes+1); // Nao existe secao 0
    manobras.resize(num_manobras+1); // Nao existe manobra 0

    // Recebe secoes
    for (int i = 1; i <= num_secoes; i++)
        cin >> secoes[i].bonificacao >> secoes[i].tempo_travessia;

    // Recebe Manobras
    for (int i = 1; i <= num_manobras; i++) {
        manobras[i].nome = i;
        cin >> manobras[i].pontuacao >> manobras[i].tempo_manobra;
    }

    // Descobre a travessia mais radical
    TravessiaMaisRadical(num_secoes, num_manobras, secoes, manobras);

    return 0;
}
*/

/*
#include <iostream>
#include <vector>

using namespace std;

#define INF 0x7FFFFFFFFFFFFFFF

struct Secao {
    int bonificacao;
    int tempo_travessia;
    vector <int> manobras;
};

struct Manobra {
    int nome;
    int pontuacao;
    int tempo_manobra;
};

// Descobre a travessia mais radical
void TravessiaMaisRadical (int num_secoes, int num_manobras, vector <Secao> secoes, vector <Manobra> manobras) {

    // Declara variaveis
    int pontuacao_max = 0;
    int capacidade = 0;
    vector<vector<int>> dp;
    vector<vector<int>> nxt;

    // Problema das machilas
    for (int i = 1; i <= num_secoes; i++) {

        capacidade = secoes[i].tempo_travessia;

        dp.resize(num_manobras+1, vector<int>(capacidade+1, 0));
        nxt.resize(num_manobras+1, vector<int>(capacidade+1, 0));

        for (int j = 0; j <= num_manobras; j++) {
            for (int k = 0; k <= capacidade; k++) {
                if (j == 0 || k==0) {
                    dp[j][k] = 0;
                    continue;
                }

                nxt[j][k] = k;
                int melhor_valor = dp[j-1][k];

                if (k >= manobras[j].tempo_manobra) {
                    int novo_valor = dp[j-1][k - manobras[j].tempo_manobra] + manobras[j].pontuacao;
                    if (novo_valor > melhor_valor) {
                        melhor_valor = novo_valor;
                        nxt[j][k] = k - manobras[j].tempo_manobra;
                    }
                }

                dp[j][k] = melhor_valor;
            }
        }

        pontuacao_max += dp[num_manobras][capacidade];

        for (int j = num_manobras; j > 0; capacidade = nxt[j][capacidade], j--) {
            if (nxt[j][capacidade] != capacidade) {
                secoes[i].manobras.insert(secoes[i].manobras.begin(), manobras[j].nome);
            }
        }

    }

    // Imprime pontuacao maxima
    cout << pontuacao_max << endl;

    // Imprime numero de manobras por secao e quais manobras executadas na secao
    for (int i = 1; i <= num_secoes; i++) {
        cout << secoes[i].manobras.size();
        for (int j = 0; j < secoes[i].manobras.size(); j++)
            cout << " " << secoes[i].manobras[j];
        cout << endl;
    }

}

int main(int argc, char const *argv[]) {

    // Declara variaaveis
    int num_secoes = 0, num_manobras = 0;
    vector <Secao> secoes;
    vector <Manobra> manobras;

    // Recebe numero de secoes e numero de manobras
    cin >> num_secoes >> num_manobras;

    // Atualiza tamanho dos vetores de secoes e manobras
    secoes.resize(num_secoes+1); // Nao existe secao 0
    manobras.resize(num_manobras+1); // Nao existe manobra 0

    // Recebe secoes
    for (int i = 1; i <= num_secoes; i++)
        cin >> secoes[i].bonificacao >> secoes[i].tempo_travessia;

    // Recebe Manobras
    for (int i = 1; i <= num_manobras; i++) {
        manobras[i].nome = i;
        cin >> manobras[i].pontuacao >> manobras[i].tempo_manobra;
    }

    // Descobre a travessia mais radical
    TravessiaMaisRadical(num_secoes, num_manobras, secoes, manobras);

    return 0;
}
*/

/*
#include <iostream>
#include <vector>

using namespace std;

#define INF 0x7FFFFFFFFFFFFFFF

struct Secao {
    int bonificacao;
    int tempo_travessia;
    vector <int> manobras;
};

struct Manobra {
    int nome;
    int pontuacao;
    int tempo_manobra;
};

// Descobre a travessia mais radical
void TravessiaMaisRadical (int num_secoes, int num_manobras, vector <Secao> secoes, vector <Manobra> manobras) {

    // Declara variaveis
    int pontuacao_max = 0;
    int capacidade = 0;
    vector<vector<int>> dp;
    vector<vector<int>> nxt;

    // Problema das machilas
    for (int i = 1; i <= num_secoes; i++) {

        capacidade = secoes[i].tempo_travessia;
    
        dp.resize(num_manobras+1, vector<int>(capacidade+1, 0));
        nxt.resize(num_manobras+1, vector<int>(capacidade+1, 0));

        for (int j = 0; j <= num_manobras; j++) {
            for (int k = 0; k <= capacidade; k++) {
                if (j == 0 || k == 0) {
                    dp[j][k] = 0;
                    continue;
                }
                else if (manobras[j].tempo_manobra <= k) {
                    dp[j][k] = max(manobras[j].pontuacao + dp[j-1][k - manobras[j].tempo_manobra], dp[j-1][k]);
                }
                else {
                    dp[j][k] = dp[j-1][k];
                }
            }
        }

        pontuacao_max += dp[num_manobras][capacidade];

        for (int j = 0; j < num_manobras; capacidade = nxt[j][capacidade], j++) {
            if (nxt[j][capacidade] != capacidade) {
                secoes[i].manobras.push_back(manobras[j].nome);
            }
        }

    }

    // Imprime pontuacao maxima
    cout << pontuacao_max << endl;

    // Imprime numero de manobras por secao e quais manobras executadas na secao
    for (int i = 1; i <= num_secoes; i++) {
        cout << secoes[i].manobras.size();
        for (int j = 0; j < secoes[i].manobras.size(); j++)
            cout << " " << secoes[i].manobras[j];
        cout << endl;
    }
    
}

int main(int argc, char const *argv[]) {

    // Declara variaaveis
    int num_secoes = 0, num_manobras = 0;
    vector <Secao> secoes;
    vector <Manobra> manobras;

    // Recebe numero de secoes e numero de manobras
    cin >> num_secoes >> num_manobras;

    // Atualiza tamanho dos vetores de secoes e manobras
    secoes.resize(num_secoes+1); // Nao existe secao 0
    manobras.resize(num_manobras+1); // Nao existe manobra 0

    // Recebe secoes
    for (int i = 1; i <= num_secoes; i++)
        cin >> secoes[i].bonificacao >> secoes[i].tempo_travessia;

    // Recebe Manobras
    for (int i = 1; i <= num_manobras; i++) {
        manobras[i].nome = i;
        cin >> manobras[i].pontuacao >> manobras[i].tempo_manobra;
    }

    // Descobre a travessia mais radical
    TravessiaMaisRadical(num_secoes, num_manobras, secoes, manobras);

    return 0;
}
*/

/*
#include <iostream>
#include <vector>

using namespace std;

struct Secao {
    int bonificacao;
    int tempo_travessia;
    vector<int> manobras;
};

struct Manobra {
    int nome;
    int pontuacao;
    int tempo_manobra;
    int ultima_execucao;
};

void TravessiaMaisRadical(int num_secoes, int num_manobras, vector<Secao> &secoes, vector<Manobra> &manobras) {

    int pontuacao_max = 0;
    int capacidade = 0;
    vector<vector<int>> dp;
    vector<vector<int>> nxt;

    for (int i = 1; i <= num_secoes; i++) {
        capacidade = secoes[i].tempo_travessia;

        dp.resize(num_manobras + 1, vector<int>(capacidade + 1, 0));
        nxt.resize(num_manobras + 1, vector<int>(capacidade + 1, 0));

        for (int j = 0; j <= num_manobras; j++) {
            for (int k = 0; k <= capacidade; k++) {
                if (j == 0 || k == 0) {
                    dp[j][k] = 0;
                    continue;
                }

                nxt[j][k] = k;
                int melhor_valor = dp[j - 1][k];

                if (k >= manobras[j].tempo_manobra) {
                    int ultima_execucao = manobras[j].ultima_execucao;
                    int fator_divisao = (i - ultima_execucao == 1) ? 2 : 1;

                    int novo_valor = dp[j - 1][k - manobras[j].tempo_manobra] + manobras[j].pontuacao / fator_divisao;

                    if (novo_valor > melhor_valor) {
                        melhor_valor = novo_valor;
                        nxt[j][k] = k - manobras[j].tempo_manobra;
                    }
                }

                dp[j][k] = melhor_valor;
            }
        }

        for (int j = num_manobras; j > 0; capacidade = nxt[j][capacidade], j--) {
            if (nxt[j][capacidade] != capacidade) {
                secoes[i].manobras.insert(secoes[i].manobras.begin(), manobras[j].nome);
                manobras[j].ultima_execucao = i;
            }
        }

        pontuacao_max += (dp[num_manobras][secoes[i].tempo_travessia] * secoes[i].bonificacao * secoes[i].manobras.size());
    }

    // Imprime pontuacao maxima
    cout << pontuacao_max << endl;

    // Imprime numero de manobras por secao e quais manobras executadas na secao
    for (int i = 1; i <= num_secoes; i++) {
        cout << secoes[i].manobras.size();
        for (int j = 0; j < secoes[i].manobras.size(); j++)
            cout << " " << secoes[i].manobras[j];
        cout << endl;
    }
}

int main(int argc, char const *argv[]) {

    // Declara variaaveis
    int num_secoes = 0, num_manobras = 0;
    vector <Secao> secoes;
    vector <Manobra> manobras;

    // Recebe numero de secoes e numero de manobras
    cin >> num_secoes >> num_manobras;

    // Atualiza tamanho dos vetores de secoes e manobras
    secoes.resize(num_secoes+1); // Nao existe secao 0
    manobras.resize(num_manobras+1); // Nao existe manobra 0

    // Recebe secoes
    for (int i = 1; i <= num_secoes; i++)
        cin >> secoes[i].bonificacao >> secoes[i].tempo_travessia;

    // Recebe Manobras
    for (int i = 1; i <= num_manobras; i++) {
        manobras[i].nome = i;
        manobras[i].ultima_execucao = -1;
        cin >> manobras[i].pontuacao >> manobras[i].tempo_manobra;
    }

    // Descobre a travessia mais radical
    TravessiaMaisRadical(num_secoes, num_manobras, secoes, manobras);

    return 0;
}
*/

/*
#include <iostream>
#include <vector>

using namespace std;

struct Secao {
    int bonificacao;
    int tempo_travessia;
    vector<int> manobras;
};

struct Manobra {
    int nome;
    int pontuacao;
    int tempo_manobra;
    int ultima_execucao;
};

void TravessiaMaisRadical(int num_secoes, int num_manobras, vector<Secao> &secoes, vector<Manobra> &manobras) {

    int pontuacao_max = 0;
    int capacidade = 0;
    vector<vector<int>> dp;
    vector<vector<int>> nxt;

    for (int i = 1; i <= num_secoes; i++) {
      cout << "teste " << i << " - inicio"<< endl;
        capacidade = secoes[i].tempo_travessia;

        dp.resize(num_manobras + 1, vector<int>(capacidade + 1, 0));
        nxt.resize(num_manobras + 1, vector<int>(capacidade + 1, 0));

      cout << "teste " << i << " - matrizes redimencionadas"<< endl;

        for (int j = 0; j <= num_manobras; j++) {
            for (int k = 0; k <= capacidade; k++) {
              cout << "teste " << i << " - j = " << j << " - k = " << k << endl;
                if (j == 0 || k == 0) {
                    dp[j][k] = 0;
                    continue;
                }

                nxt[j][k] = k;
                int melhor_valor = dp[j - 1][k];

                if (k >= manobras[j].tempo_manobra) {
                    int ultima_execucao = manobras[j].ultima_execucao;
                    int fator_divisao = (i - ultima_execucao == 1) ? 2 : 1;

                    int novo_valor = dp[j - 1][k - manobras[j].tempo_manobra] + manobras[j].pontuacao / fator_divisao;

                    if (novo_valor > melhor_valor) {
                        melhor_valor = novo_valor;
                        nxt[j][k] = k - manobras[j].tempo_manobra;
                    }
                }

                dp[j][k] = melhor_valor;
            }
        }

      cout << "teste " << i << " - preencheu dp"<< endl;

        for (int j = num_manobras; j > 0; capacidade = nxt[j][capacidade], j--) {
            if (nxt[j][capacidade] != capacidade) {
                secoes[i].manobras.insert(secoes[i].manobras.begin(), manobras[j].nome);
                manobras[j].ultima_execucao = i;
            }
        }

        pontuacao_max += (dp[num_manobras][secoes[i].tempo_travessia] * secoes[i].bonificacao * secoes[i].manobras.size());

      cout << "teste " << i << " - fim"<< endl;
    }
    cout << "Saiu do loop" << endl;
    // Imprime pontuacao maxima
    cout << pontuacao_max << endl;

    // Imprime numero de manobras por secao e quais manobras executadas na secao
    for (int i = 1; i <= num_secoes; i++) {
        cout << secoes[i].manobras.size();
        for (int j = 0; j < secoes[i].manobras.size(); j++)
            cout << " " << secoes[i].manobras[j];
        cout << endl;
    }
}

int main(int argc, char const *argv[]) {

    // Declara variaaveis
    int num_secoes = 0, num_manobras = 0;
    vector <Secao> secoes;
    vector <Manobra> manobras;

    // Recebe numero de secoes e numero de manobras
    cin >> num_secoes >> num_manobras;

    // Atualiza tamanho dos vetores de secoes e manobras
    secoes.resize(num_secoes+1); // Nao existe secao 0
    manobras.resize(num_manobras+1); // Nao existe manobra 0

    // Recebe secoes
    for (int i = 1; i <= num_secoes; i++)
        cin >> secoes[i].bonificacao >> secoes[i].tempo_travessia;

    // Recebe Manobras
    for (int i = 1; i <= num_manobras; i++) {
        manobras[i].nome = i;
        manobras[i].ultima_execucao = -1;
        cin >> manobras[i].pontuacao >> manobras[i].tempo_manobra;
    }

    // Descobre a travessia mais radical
    TravessiaMaisRadical(num_secoes, num_manobras, secoes, manobras);

    return 0;
}
*/

/*
1 3
10 20
50 8
90 11
100 10
*/

/*
3 2
10 20
1 60
100 60
50 10
1000 50
*/

/*
3 2
4 1
3 3
1 1
1000 1
50 2
*/

/*
10 4
3 84
3 100
5 16
5 82
7 4
2 48
9 11
10 37
9 73
4 14
4 18
20 19
13 12
6 13
*/

#include <iostream>
#include <vector>

using namespace std;

struct Secao {
    int bonificacao;
    int tempo_travessia;
    vector<int> manobras;
};

struct Manobra {
    int nome;
    int pontuacao;
    int tempo_manobra;
    int ultima_execucao;
};

void TravessiaMaisRadical(int num_secoes, int num_manobras, vector<Secao> &secoes, vector<Manobra> &manobras) {

    int pontuacao_max = 0;
    int capacidade = 0;
    vector<vector<int>> dp;
    vector<vector<int>> mem_manobras;

    for (int i = 1; i <= num_secoes; i++) {
        
        capacidade = secoes[i].tempo_travessia;

        dp.resize(num_manobras + 1, vector<int>(capacidade + 1, 0));
        mem_manobras.resize(num_manobras + 1, vector<int>(capacidade + 1, 0));

        for (int j = 0; j <= num_manobras; j++) {
            for (int k = 0; k <= capacidade; k++) {
                
                if (j == 0 || k == 0) {
                    dp[j][k] = 0;
                    continue;
                }

                mem_manobras[j][k] = k;
                int melhor_valor = dp[j - 1][k];

                if (k >= manobras[j].tempo_manobra) {
                    int ultima_execucao = manobras[j].ultima_execucao;
                    int fator_divisao = (i - ultima_execucao == 1) ? 2 : 1;

                    int novo_valor = dp[j - 1][k - manobras[j].tempo_manobra] + manobras[j].pontuacao / fator_divisao;

                    if (novo_valor > melhor_valor) {
                        melhor_valor = novo_valor;
                        mem_manobras[j][k] = k - manobras[j].tempo_manobra;
                    }
                }

                dp[j][k] = melhor_valor;
            }
        }

        for (int j = num_manobras; j > 0; capacidade = mem_manobras[j][capacidade], j--) {
            if (mem_manobras[j][capacidade] != capacidade) {
                secoes[i].manobras.insert(secoes[i].manobras.begin(), manobras[j].nome);
                manobras[j].ultima_execucao = i;
            }
        }

        pontuacao_max += (dp[num_manobras][secoes[i].tempo_travessia] * secoes[i].bonificacao * secoes[i].manobras.size());
    }
    
    // Imprime pontuacao maxima
    cout << pontuacao_max << endl;

    // Imprime numero de manobras por secao e quais manobras executadas na secao
    for (int i = 1; i <= num_secoes; i++) {
        cout << secoes[i].manobras.size();
        for (int j = 0; j < secoes[i].manobras.size(); j++)
            cout << " " << secoes[i].manobras[j];
        cout << endl;
    }  
}

int main(int argc, char const *argv[]) {

    // Declara variaaveis
    int num_secoes = 0, num_manobras = 0;
    vector <Secao> secoes;
    vector <Manobra> manobras;

    // Recebe numero de secoes e numero de manobras
    cin >> num_secoes >> num_manobras;

    // Atualiza tamanho dos vetores de secoes e manobras
    secoes.resize(num_secoes+1); // Nao existe secao 0
    manobras.resize(num_manobras+1); // Nao existe manobra 0

    // Recebe secoes
    for (int i = 1; i <= num_secoes; i++)
        cin >> secoes[i].bonificacao >> secoes[i].tempo_travessia;

    // Recebe Manobras
    for (int i = 1; i <= num_manobras; i++) {
        manobras[i].nome = i;
        manobras[i].ultima_execucao = -1;
        cin >> manobras[i].pontuacao >> manobras[i].tempo_manobra;
    }

    // Descobre a travessia mais radical
    TravessiaMaisRadical(num_secoes, num_manobras, secoes, manobras);

    return 0;
}

/*
0 - 0000
1 - 0001
2 - 0010
3 - 0011
4 - 0100
5 - 0101
6 - 0110
7 - 0111
8 - 1000
9 - 1001
A - 1010
B - 1011
C - 1100
D - 1101
E - 1110
F - 1111
*/
