#include <iostream>
#include <vector>

using namespace std;

struct Secao {
    int bonificacao;
    int tempo_travessia;
    vector <int> manobras;
};

struct Manobra {
    int nome;
    int pontuacao;
    int tempo_manobra;
    int ultima_execucao;
};

// Descobre a travessia mais radical
void TravessiaMaisRadical (int num_secoes, int num_manobras, vector <Secao> secoes, vector <Manobra> manobras) {

    // Declara variaveis
    int pontuacao_max = 0;
    int capacidade = 0;
    vector<vector<int>> dp;
    vector<vector<int>> mem_manobras;

    // Problema das machilas
    for (int i = 1; i <= num_secoes; i++) {

        capacidade = secoes[i].tempo_travessia;

        dp.resize(num_manobras+1, vector<int>(capacidade+1, 0));
        mem_manobras.resize(num_manobras+1, vector<int>(capacidade+1, 0));

        for (int j = 0; j <= num_manobras; j++) {
            for (int k = 0; k <= capacidade; k++) {
                if (j == 0 || k==0) {
                    dp[j][k] = 0;
                    continue;
                }

                mem_manobras[j][k] = k;
                int melhor_valor = dp[j-1][k];

                if (k >= manobras[j].tempo_manobra) {
                    int novo_valor = dp[j-1][k - manobras[j].tempo_manobra] + manobras[j].pontuacao;
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