# Nome do executável
TARGET = tp1

# Diretórios
SRC_DIR = graph_search/src
INC_DIR = graph_search/include
BUILD_DIR = build
DATA_DIR = data

# Arquivos fonte
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Arquivos objeto (ficam na pasta build)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -I$(INC_DIR) -Wall -Wextra

# Regra padrão: compila tudo
all: build

build: $(BUILD_DIR)/$(TARGET)

# Compilação final do executável
$(BUILD_DIR)/$(TARGET): $(OBJS)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CXX) $(OBJS) -o $(BUILD_DIR)/$(TARGET)

# Compila os .cpp para .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpa apenas a pasta build
clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)

# Limpa build e data
clean_all: clean
	@if exist $(DATA_DIR) rmdir /s /q $(DATA_DIR)

.PHONY: all build clean clean_all