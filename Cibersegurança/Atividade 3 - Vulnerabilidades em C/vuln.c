// vuln.c  (exemplo didático)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_input(const char *s, size_t count) {
    // erro: cálculo de tamanho pode overflowar
    size_t total = count * sizeof(char);

    // VULNERABILIDADE 1 (Integer Overflow):
    // Se 'total' for SIZE_MAX, 'total + 1' estoura e vira 0.
    char *buf = malloc(total + 1);

    if (!buf) { perror("malloc"); exit(1); }

    // erro: copia sem checar limites

    // VULNERABILIDADE 2 (Buffer Overflow):
    // 'strcpy' não sabe o tamanho de 'buf'. Ele copia 's' até
    // encontrar um '\0', mesmo que 's' seja maior que 'buf'.
    strcpy(buf, s);
    
    printf("Recebi: %s\n", buf);
    free(buf);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "uso: %s <texto> <count>\n", argv[0]);
        return 1;
    }
    size_t count = (size_t) strtoul(argv[2], NULL, 10);
    process_input(argv[1], count);
    return 0;
}