#include "termo.hpp"
#include "indice.hpp"
#include "consulta.hpp"
#include "arq_funcao.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

void show_help(char *name) {
    cout << name << " <opcoes>\n\
            -h         mostra esta tela e sai.\n\
            -i consuta.txt      seta o seu nome do arquivo de consulta.\n\
            -o ranking.txt      seta o seu nome do arquivo com o ranking.\n\
            -c corpus           seta o nome da pasta corpus.\n\
            -s stopwords.txt    seta o seu nome do arquivo com as stopwords.\n" << endl;
    exit(-1) ;
}

int main(int argc, char *argv[]) {

    // Parametros de entrada
    string arq_consulta;
    string arq_ranking;
    string pasta_corpus;
    string arq_stopwords;

    // Recebe os parametros passados pela linha de comando
    int opt;

    while( (opt = getopt(argc, argv, "hi:o:c:s:")) > 0 ) {
        switch ( opt ) {
            case 'h': // help
                show_help(argv[0]);
                break;
            case 'i': // opção -i
                arq_consulta = optarg;
                break;
            case 'o': // opção -o 
                arq_ranking = optarg;
                break;
            case 'c': // opção -c 
                pasta_corpus = optarg;
                break;
            case 's': // opção -s 
                arq_stopwords = optarg;
                break;
            default:
                cout << "Opcao invalida ou faltando argumento: " << optopt << endl;
                return -1 ;
        }
    }

    // Confere se tem argumentos em excesso 
    if ( argv[optind] != NULL ) {
        int i;

        cout << endl << "** Argumentos em excesso **" << endl << endl;
        for(i=optind; argv[i] != NULL; i++) {
            cout << "-- " << argv[i] << endl;
        }
        cout << endl;
        show_help(argv[0]);
    }

    // Variaveis
    int num_arq = 0;    // Numero de arquivos na pasta
    int tam_vocabulario = 0;    // Numero de palavras do vocabulario
    
    // Conta quantos arquivos tem no corpus
    cout << "Conta Arquivos: ";
    num_arq = ContaArquivos(pasta_corpus);
    cout << "OK" << endl;

    // Conta quantas palavras tem o vocabulario e trata os arquivos
    cout << "Prepara corpus: " << endl;
    tam_vocabulario = PreparaCorpus(pasta_corpus, arq_stopwords);
    cout << "OK" << endl;
    
    // Cria o indice invertido
    cout << "Indice ";
    Termo* indice = new Termo[tam_vocabulario];
    CriaIndice(arq_consulta, arq_ranking, pasta_corpus, arq_stopwords, num_arq, tam_vocabulario, indice);    
    cout << "OK"<< endl;
    

    // Realiza a consulta
    cout << "Consulta: ";
    Consulta(arq_ranking, arq_consulta, indice, num_arq, tam_vocabulario);
    cout << "OK" << endl;

    // Desaloca indice
    delete[] indice;
   
    return 0;
}