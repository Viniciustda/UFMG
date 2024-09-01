#---------------------------------------------------------------------
# Arquivo	: Makefile
# Conteúdo	: compilar o tp2
# Autor		: Vinicius Trindade Dias Abel (viniciustda@ufmg.br)
# Histórico	: 2022-01-13 - arquivo criado
#---------------------------------------------------------------------
# Opções	: make all - compila tudo
#		: make clean - remove objetos e executável
#---------------------------------------------------------------------

CC = g++
LIBS = -lm
SRC = src
OBJ = obj
INC = include
BIN = bin
OBJS = $(OBJ)/arq.o $(OBJ)/filaencadeada.o $(OBJ)/arq_funcao.o $(OBJ)/entidade.o $(OBJ)/heap.o $(OBJ)/quicksort.o $(OBJ)/primeira_etapa.o $(OBJ)/segunda_etapa.o $(OBJ)/tp2.o
HDRS = $(INC)/arq.hpp $(INC)/filaencadeada.hpp $(INC)/arq_funcao.hpp $(INC)/entidade.hpp $(INC)/heap.hpp $(INC)/quicksort.hpp $(INC)/primeira_etapa.hpp $(INC)/segunda_etapa.hpp
CFLAGS = -Wall -c -I$(INC)

EXE = $(BIN)/tp2

all: $(EXE)

$(BIN)/tp2: $(OBJS)
	$(CC) -o $(BIN)/tp2 $(OBJS) $(LIBS)

$(OBJ)/tp2.o: $(HDRS) $(SRC)/tp2.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/tp2.o $(SRC)/tp2.cpp 

$(OBJ)/segunda_etapa.o: $(HDRS) $(SRC)/segunda_etapa.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/segunda_etapa.o $(SRC)/segunda_etapa.cpp

$(OBJ)/primeira_etapa.o: $(HDRS) $(SRC)/primeira_etapa.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/primeira_etapa.o $(SRC)/primeira_etapa.cpp

$(OBJ)/quicksort.o: $(HDRS) $(SRC)/quicksort.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/quicksort.o $(SRC)/quicksort.cpp

$(OBJ)/heap.o: $(HDRS) $(SRC)/heap.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/heap.o $(SRC)/heap.cpp

$(OBJ)/entidade.o: $(HDRS) $(SRC)/entidade.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/entidade.o $(SRC)/entidade.cpp

$(OBJ)/arq_funcao.o: $(HDRS) $(SRC)/arq_funcao.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/arq_funcao.o $(SRC)/arq_funcao.cpp

$(OBJ)/filaencadeada.o: $(HDRS) $(SRC)/filaencadeada.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/filaencadeada.o $(SRC)/filaencadeada.cpp

$(OBJ)/arq.o: $(HDRS) $(SRC)/arq.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/arq.o $(SRC)/arq.cpp

clean:
	rm $(EXE) $(OBJS) 