# Makefile para o projeto de RPC com Portmapper e Registry

# Define o interpretador Python a ser usado
PYTHON = python3

# Comando para o compilador de .proto do gRPC
PROTOC = $(PYTHON) -m grpc_tools.protoc

# --- Regras Principais ---

# Regra para gerar o código Python a partir dos arquivos .proto
.PHONY: generate
generate:
	@$(PROTOC) -I. --python_out=. --grpc_python_out=. portmapper/portmapper.proto registry/registry.proto

# Regra para executar o servidor Registry
# Depende de 'generate' para garantir que o código gRPC exista.
.PHONY: run_registry
run_registry: generate
	@$(PYTHON) -m registry.registry_server $(arg)

# Regra para executar o Portmapper no modo da Etapa 1 (sozinho)
.PHONY: run_portmapper_1
run_portmapper_1: generate
	@$(PYTHON) -m portmapper.portmapper_server $(arg)

# Regra para executar o Portmapper no modo da Etapa 2 (integrado)
.PHONY: run_portmapper_2
run_portmapper_2: generate
	@$(PYTHON) -m portmapper.portmapper_server $(arg1) $(arg2)

# Regra para executar o cliente do Registry
.PHONY: run_cli_registry
run_cli_registry: generate
	@$(PYTHON) -m registry.registry_client $(arg)

# Regra para executar o cliente do Portmapper
.PHONY: run_cli_portmapper
run_cli_portmapper: generate
	@$(PYTHON) -m portmapper.portmapper_client $(arg)

# Regra para limpar todos os arquivos gerados e caches
.PHONY: clean
clean:
	@rm -f portmapper/*_pb2*.py registry/*_pb2*.py
	@rm -rf */__pycache__
	@rm -rf __pycache__