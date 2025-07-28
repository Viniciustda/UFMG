# Diretórios
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Arquivos fonte e objeto, excluindo passa_tempo.c
SRCS = $(filter-out $(SRC_DIR)/passa_tempo.c, $(wildcard $(SRC_DIR)/*.c))
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Compilador e flags
CC = gcc
CFLAGS = -Wall -I$(INC_DIR) -pthread

# Nome do executável
TARGET = $(BUILD_DIR)/ex1

# Regra padrão
all: build

# Compilação
build: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all build clean