# Makefile para o projeto "tp2virtual"

# Compilador e flags
CC = gcc
CFLAGS = -Wall -lm

# Nome do executável
EXEC = tp2virtual.exe

# Objetos necessários
OBJ = tp2virtual.o inicializacao.o listas.o auxiliares.o algoritmos.o simulacao.o

# Regra principal
all: $(EXEC)

# Linkagem do executável
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

# Regras de compilação
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Limpeza
clean:
	rm -f *.o $(EXEC)

# Regra para limpar apenas os objetos
clean-obj:
	rm -f *.o
