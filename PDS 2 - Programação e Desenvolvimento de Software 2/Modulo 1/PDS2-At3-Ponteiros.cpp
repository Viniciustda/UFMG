#include <iostream>

#define SIZE 5

int main(){
    long value1=200000;
    long value2;
    
    long *long_ptr = &value1;
    
    //imprima o valor do objeto apontado por long_ptr
    std::cout << *long_ptr << ' ';
    
    value2 = *long_ptr;
    
    //Imprima o valor de value2
    std::cout << value2 << ' ';
    
    //Imprima o endereço de value1
    std::cout << &value1 << ' ';
    
    //Imprima o endereço armazenado em long_ptr
    std::cout << long_ptr << ' ';
    
    //Imprima a comparação entre o endereço de value1 com o endereço armazenado em long_ptr
    if (&value1 == long_ptr)
        std::cout << 1 << ' ';
    else
        std::cout << 0 << ' ';
        
    unsigned int values[SIZE] = {2, 4, 6, 8, 10};
    
    unsigned int *v_ptr = nullptr;
        
    //imprimir os elementos do array values usando notação de array subscrito.
    for (int i = 0; i < SIZE; i++) {
        std::cout << values[i] << ' ';
    }
    
    v_ptr = new unsigned int[SIZE];
    for (int i = 0; i < SIZE; i++) {
        v_ptr[i] = values[i];
    }
    v_ptr = values;
    
    //imprimir os elementos do array values usando notação de array ponteiro/deslocamento.
    for (int i = 0; i < SIZE; i++) {
      std::cout << *(v_ptr + i) << ' ';
    }
    
    //imprimir os elementos do array values usando notação de array ponteiro/deslocamento com o nome de array como o ponteiro.
    for (int i = 0; i < SIZE; i++) {
       std::cout << *(values + i) << ' ';
    }
    
    //imprimir os elementos do array values utilizando subscritos no ponteiro para o array.
    for (int i = 0; i < SIZE; i++) {
      std::cout << v_ptr[i] << ' ';
    }
    
    //imprimir o quinto elemento de values utilizando a notação de subscrito de array,
    std::cout << values[4] << ' ';
    
    //a notação de ponteiro/deslocamento com o nome de array como o ponteiro, 
    std::cout << *(values + 4) << ' ';

    //a notação de subscrito de ponteiro,
    std::cout << v_ptr[4] << ' ';
    
    //a notação de ponteiro/deslocamento.
    std::cout << *(v_ptr + 4) << ' ';
    
    std::cout << (v_ptr + 3) << ' ' << *(v_ptr + 3) << ' ';
    
    //imprimir a comparação entre o valor armazenado em (v_ptr-4) e values[0]
    
    v_ptr = &values[4];
    v_ptr -= 4;
    std::cout << v_ptr << ' ' << *v_ptr;
    
    return 0;
}