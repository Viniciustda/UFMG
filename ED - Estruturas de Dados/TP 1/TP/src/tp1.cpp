#include "url.hpp"
#include "host.hpp"
#include "lista.hpp"
#include "fila.hpp"
#include "escalonador.hpp"
#include <iostream>
#include <fstream>
#include <string>


using namespace std;

int main(int argc, char const *argv[]) {

    
    Escalonador escalonador;
    string comando;
    string host;
    string nome_arq_saida;
    string entrada_url;
    int n = 0;
    int indice = 0;

    // Salva o nome do arquivo de saida
    // Que eh o mesmo do de entrada, com "-out" no final
    nome_arq_saida = argv[1];
    for (int i = 0; i < 4; i++)    
        nome_arq_saida.pop_back();
    nome_arq_saida += "-out.txt";
    escalonador.NomeArqSaida(nome_arq_saida);

    // Abre o arquivo de entrada
    // Passado pela linha de comando
    ifstream arquivo_entrada;
    arquivo_entrada.open(argv[1]);
    if (! arquivo_entrada) {
        cout << "Arquivo de entrada nao pode ser aberto" << endl;
        abort();
    }
    
    // Funcoes do programa:
    string ADD_URLS = "ADD_URLS";
    string ESCALONA_TUDO = "ESCALONA_TUDO";
    string ESCALONA = "ESCALONA";
    string ESCALONA_HOST = "ESCALONA_HOST";
    string VER_HOST = "VER_HOST";
    string LISTA_HOSTS = "LISTA_HOSTS";
    string LIMPA_HOST = "LIMPA_HOST";
    string LIMPA_TUDO = "LIMPA_TUDO";

    // O programa roda enqunto nao chegar no final do arquivo de entrada
    while(!arquivo_entrada.eof()) {
        getline(arquivo_entrada, comando);

        if (comando.find(ADD_URLS) != string::npos) {                    // Confere se este foi o comando passado. Se sim, olha quantas urls serao passadas e chama a funcao uma vez para cada url passada
            
            comando.erase(0,9);
            n = stoi(comando);
            for (int i = 0; i < n; i++) {
                getline(arquivo_entrada, entrada_url);
                escalonador.ADD_URLS(entrada_url);
            }
        }
        else if (comando.find(ESCALONA_TUDO) != string::npos) {         // Confere se este foi o comando passado. Se sim, executa a funcao que escalona todas as urls
            escalonador.ESCALONA_TUDO();
        }
        else if (comando.find(ESCALONA_HOST) != string::npos) {         // Confere se este foi o comando passado. Se sim, escalona apenas urls deste host
            comando.erase(0,14);
            host = comando;
            indice = host.find_first_of(' ',0);
            host.erase(indice,host.length()-indice);

            comando.erase(0,indice+1);
            n = stoi(comando);

            escalonador.ESCALONA_HOST(host,n);
        }
        else if (comando.find(VER_HOST) != string::npos) {              // Confere se este foi o comando passado. Se sim, mostra todas as urls do host passado
            comando.erase(0,9);
            escalonador.VER_HOST(comando);
        }
        else if (comando.find(LISTA_HOSTS) != string::npos) {           // Confere se este foi o comando passado. Se sim, mostra todos os hosts
            escalonador.LISTA_HOSTS();
        }
        else if (comando.find(LIMPA_HOST) != string::npos) {            // Confere se este foi o comando passado. Se sim, limpa todas as urls do host
            comando.erase(0,11);
            escalonador.LIMPA_HOST(comando);
        }
        else if (comando.find(LIMPA_TUDO) != string::npos) {            // Confere se este foi o comando passado. Se sim, limpa todos os hosts e suas urls
            escalonador.LIMPA_TUDO();
        }
        else if (comando.find(ESCALONA) != string::npos) {              // Confere se este foi o comando passado. Se sim, escalona determinada quantidade de urls,
            comando.erase(0,9);                                         // partindo do primeiro host e se preciso seguindo para os proximos hosts ate completar a quantidade especificada
            n = stoi(comando);
            escalonador.ESCALONA(n);
        }                                                               // Se for dado um comando invalido, o programa apenas ignora e vai para a proxima linha
    }
   
    arquivo_entrada.close();
    return 0;
}