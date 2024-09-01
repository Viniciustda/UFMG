#---------------------------------------------------------------------
# Arquivo	: Makefile
# Conteúdo	: compilar o tp1
# Autor		: Vinicius Trindade Dias Abel (viniciustda@ufmg.br)
# Histórico	: 2021-12-10 - arquivo criado
#---------------------------------------------------------------------
# Opções	: make all - compila tudo
#		: make clean - remove objetos e executável
#---------------------------------------------------------------------

CC = g++
CFLAGS = -Wall -c
LIBS = -lm
SRC = src
OBJ = obj
INC = include
BIN = bin
OBJS = $(OBJ)/funcao_arq.o $(OBJ)/url.o $(OBJ)/lista.o $(OBJ)/host.o $(OBJ)/fila.o $(OBJ)/escalonador.o $(OBJ)/tp1.o
HDRS = $(INC)/funcao_arq.hpp $(INC)/url.hpp $(INC)/lista.hpp $(INC)/host.hpp $(INC)/fila.hpp $(INC)/escalonador.hpp
CFLAGS = -Wall -c -I$(INC)

EXE = $(BIN)/tp1

all: $(EXE)

$(BIN)/tp1: $(OBJS)
	$(CC) -o $(BIN)/tp1 $(OBJS) $(LIBS)

$(OBJ)/tp1.o: $(HDRS) $(SRC)/tp1.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/tp1.o $(SRC)/tp1.cpp 

$(OBJ)/escalonador.o: $(HDRS) $(SRC)/escalonador.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/escalonador.o $(SRC)/escalonador.cpp

$(OBJ)/fila.o: $(HDRS) $(SRC)/fila.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/fila.o $(SRC)/fila.cpp

$(OBJ)/host.o: $(HDRS) $(SRC)/host.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/host.o $(SRC)/host.cpp

$(OBJ)/lista.o: $(HDRS) $(SRC)/lista.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/lista.o $(SRC)/lista.cpp

$(OBJ)/url.o: $(HDRS) $(SRC)/url.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/url.o $(SRC)/url.cpp

$(OBJ)/funcao_arq.o: $(HDRS) $(SRC)/funcao_arq.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/funcao_arq.o $(SRC)/funcao_arq.cpp

clean:
	rm $(EXE) $(OBJS) 