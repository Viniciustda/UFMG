// vuln_fixed.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> // Necessário para SIZE_MAX

void process_input_safe(const char *s, size_t count) {
    
    // CORREÇÃO 1: Prevenção do Integer Overflow
    // Verifica se 'count' já está no valor máximo.
    // Se estiver, 'count + 1' causaria um overflow.
    if (count >= SIZE_MAX) { 
        fprintf(stderr, "Erro: Valor de 'count' muito grande, causa overflow.\n"); 
        exit(1); 
    }

    // O tamanho total necessário é 'count' bytes + 1 byte para o '\0'
    size_t total_size = count + 1;
    char *buf = malloc(total_size);
    if (!buf) { 
        perror("malloc"); 
        exit(1); 
    }

    // CORREÇÃO 2: Prevenção do Buffer Overflow
    // Substitui 'strcpy' por 'strncpy'.
    // Copia no máximo 'total_size - 1' (ou seja, 'count') caracteres.
    strncpy(buf, s, total_size - 1); 
    
    // GARANTIA DE SEGURANÇA:
    // 'strncpy' não garante o terminador nulo se a string 's'
    // for maior que o buffer. Força o último byte a ser '\0'.
    buf[total_size - 1] = '\0'; 

    printf("Recebi (seguro): %s\n", buf);
    free(buf);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "uso: %s <texto> <count>\n", argv[0]);
        return 1;
    }
    size_t count = (size_t) strtoul(argv[2], NULL, 10);
    
    // Chamando a função segura
    process_input_safe(argv[1], count);
    return 0;
}