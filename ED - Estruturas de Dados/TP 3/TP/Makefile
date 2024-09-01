#---------------------------------------------------------------------
# Arquivo	: Makefile
# Conteúdo	: compilar o tp3
# Autor		: Vinicius Trindade Dias Abel (viniciustda@ufmg.br)
# Histórico	: 2022-02-20 - arquivo criado
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
OBJS = $(OBJ)/arq.o $(OBJ)/filaencadeada.o $(OBJ)/arq_funcao.o $(OBJ)/frequencia.o $(OBJ)/lista.o $(OBJ)/termo.o $(OBJ)/hashing.o $(OBJ)/indice.o $(OBJ)/similaridade.o $(OBJ)/consulta.o $(OBJ)/buscar.o
HDRS = $(INC)/arq.hpp $(INC)/filaencadeada.hpp $(INC)/arq_funcao.hpp $(INC)/frequencia.hpp $(INC)/lista.hpp $(INC)/termo.hpp $(INC)/hashing.hpp $(INC)/indice.hpp $(INC)/similaridade.hpp $(INC)/consulta.hpp
CFLAGS = -Wall -c -I$(INC)

EXE = $(BIN)/buscar

all: $(EXE)

$(BIN)/buscar: $(OBJS)
	$(CC) -o $(BIN)/buscar $(OBJS) $(LIBS)

$(OBJ)/buscar.o: $(HDRS) $(SRC)/buscar.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/buscar.o $(SRC)/buscar.cpp 

$(OBJ)/consulta.o: $(HDRS) $(SRC)/consulta.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/consulta.o $(SRC)/consulta.cpp

$(OBJ)/similaridade.o: $(HDRS) $(SRC)/similaridade.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/similaridade.o $(SRC)/similaridade.cpp

$(OBJ)/indice.o: $(HDRS) $(SRC)/indice.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/indice.o $(SRC)/indice.cpp

$(OBJ)/hashing.o: $(HDRS) $(SRC)/hashing.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/hashing.o $(SRC)/hashing.cpp

$(OBJ)/termo.o: $(HDRS) $(SRC)/termo.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/termo.o $(SRC)/termo.cpp

$(OBJ)/lista.o: $(HDRS) $(SRC)/lista.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/lista.o $(SRC)/lista.cpp

$(OBJ)/frequencia.o: $(HDRS) $(SRC)/frequencia.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/frequencia.o $(SRC)/frequencia.cpp

$(OBJ)/arq_funcao.o: $(HDRS) $(SRC)/arq_funcao.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/arq_funcao.o $(SRC)/arq_funcao.cpp

$(OBJ)/filaencadeada.o: $(HDRS) $(SRC)/filaencadeada.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/filaencadeada.o $(SRC)/filaencadeada.cpp

$(OBJ)/arq.o: $(HDRS) $(SRC)/arq.cpp
	$(CC) $(CFLAGS) -o $(OBJ)/arq.o $(SRC)/arq.cpp

clean:
	rm $(EXE) $(OBJS) 