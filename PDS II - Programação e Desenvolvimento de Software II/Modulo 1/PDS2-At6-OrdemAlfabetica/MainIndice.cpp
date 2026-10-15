#include "Indice.hpp"

int main() {

    Texto text;
    string entrada;
    int tamanho = 0;
   
   while (std::cin >> entrada) {

        for (unsigned int i = 0; i < entrada.size(); i++) {
            if (entrada[i] >= 65 && entrada[i] <= 90) {
                entrada[i] += 32;
            }
        }

        text.palavra.push_back(entrada);

        tamanho++;

        text.palavra.sort();

        text.palavra.remove_if(pequeno);

        std::list<string>::iterator it;
        std::list<string>::iterator it2;

        it = text.palavra.begin();
        it2 = text.palavra.begin();
        do {
            
            if (it == text.palavra.begin()) {
                cout << *it << ' ';
                cout << text.repeticoes(tamanho, text.palavra, *it) << ' ';
                cout << fixed << setprecision(2) << text.Percentual(tamanho, text.palavra, *it) << endl;
                ++it;
            } else if (*it != *it2) {
                cout << *it << ' ';
                cout << text.repeticoes(tamanho, text.palavra, *it) << ' ';
                cout << fixed << setprecision(2) << text.Percentual(tamanho, text.palavra, *it) << endl;
                ++it;
                ++it2;
            } else {
                ++it;
                ++it2;
            }

        } while (it != text.palavra.end());
        cout << endl;
        
   }
    
    return 0;    
}

//g++ MainIndice.cpp Indice.cpp -o teste



//text.palavra.unique();


/*for (it = text.palavra.begin(); it != text.palavra.end(); ++it) {
     if (*it != *(it--)) {
        cout << *it << ' ';
        cout << text.repeticoes(tamanho, text.palavra, *it) << ' ';
        cout << fixed << setprecision(2) << text.Percentual(tamanho, text.palavra, *it) << endl;
    }
}
cout << endl;*/