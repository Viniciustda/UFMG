#include <iostream>
#include <string>

using namespace std;

class Exception {
  private:
    string erro;

  public:
    void setErro(string mensagem) {
        this->erro = mensagem;
    }

    string getErro() {
      return erro;
    }
};

class IndiceNaoInicializado : public Exception {
  public:
    IndiceNaoInicializado() {
      setErro("Erro: indice nao inicializado.");
    }
};

class IndiceNegativo : public Exception {
  public:
    IndiceNegativo() {
      setErro("Erro: indice negativo.");
    }
};

class IndiceMaiorQueArranjo : public Exception {
  public:
    IndiceMaiorQueArranjo() {
      setErro("Erro: indice maior que arranjo.");
    }
};

template <class T, int N> class BoundedArray {
  public:

    void set(int index, T value) {
      if (index < 0) throw IndiceNegativo();
      if (index >= N) throw IndiceMaiorQueArranjo();
        
      buf[index] = value;
      inicializado[index] = true;
    }

    T get(int index) {
        if (index < 0) throw IndiceNegativo();
        if (index >= N) throw IndiceMaiorQueArranjo();
        if (inicializado[index] == false) throw IndiceNaoInicializado();

        return buf[index];
    }

  private:
    T buf[N];
    bool inicializado[N] = {false};
};

template <class T> void testArray() {
  BoundedArray<T, 8> a;
  char action;
  while (cin >> action) {
    int index;
    cin >> index;
    try {
      if (action == 's') {
        T value;
        cin >> value;
        a.set(index, value);
      } else if (action == 'g') {
        cout << a.get(index) << endl;
      }
    } catch(IndiceNegativo e) {
        cerr << e.getErro() << endl;
      } catch(IndiceMaiorQueArranjo e) {
        cerr << e.getErro() << endl;
      } catch(IndiceNaoInicializado e) {
        cerr << e.getErro() << endl;
      } catch (...) {
        cerr << "Erro desconhecido." << endl;
    }
  }
}

int main() {
  char type;
  cin >> type;
  switch(type) {
    case 'd':
      testArray<double>();
      break;
    case 'i':
      testArray<int>();
      break;
    case 's':
      testArray<string>();
      break;
  }
  return 0;
}