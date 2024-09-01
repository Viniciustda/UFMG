#include <iostream>

int main() {
    int entrada;

    std::cin >> entrada;
    for (int i = 1; i <= entrada; i++) {
        for (int j = 0; j < i; j++) {
            std::cout << '*';
        }
        std::cout << std::endl;
    }
    for (int i = entrada; i > 1; i--) {
        for (int j = i-1; j > 0; j--) {
            std::cout << '*';
        }
        std::cout << std::endl;
    }
}