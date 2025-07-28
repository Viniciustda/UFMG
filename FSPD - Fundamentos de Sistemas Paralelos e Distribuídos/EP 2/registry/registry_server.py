# registry_server.py

# Módulos padrão do Python
import grpc
from concurrent import futures
import sys
import threading
import time

# Módulos gerados pelo gRPC a partir do registry.proto.
from . import registry_pb2
from . import registry_pb2_grpc


class RegistryServicer(registry_pb2_grpc.RegistryServicer):
    """
    Esta classe implementa a lógica do serviço Registry.
    Ela herda da classe base gerada pelo gRPC e implementa os métodos RPC.
    """

    def __init__(self, server_instance):
        """
        Construtor do Servicer. Inicializa o estado do servidor.
        :param server_instance: A referência ao objeto principal do servidor gRPC.
        """
        # --- Estado do Servidor ---
        # O dicionário principal que atua como o diretório de serviços.
        # Mapeia: nome do serviço (string) -> localizador do Portmapper (string "host:porta")
        self.registry = {}
        # Lock para garantir que o acesso ao dicionário seja seguro.
        self.lock = threading.Lock()

        # --- Configuração ---
        # Referência ao servidor gRPC, usada para o shutdown.
        self.server = server_instance

    def Map(self, request, context):
        """Implementação do RPC Map. Adiciona ou atualiza uma entrada no diretório."""
        # Garante acesso exclusivo ao dicionário compartilhado.
        with self.lock:
            # Insere ou sobrescreve a chave (nome do serviço) com o novo valor (localizador do Portmapper).
            self.registry[request.service_name] = request.portmapper_locator
            count = len(self.registry)
        return registry_pb2.MapResponse(mapped_services_count=count)

    def Unmap(self, request, context):
        """Implementação do RPC Unmap. Remove uma entrada do diretório."""
        with self.lock:
            # Verifica se o serviço está registrado antes de tentar removê-lo.
            if request.service_name in self.registry:
                del self.registry[request.service_name]
                # Retorna 0 em caso de sucesso.
                return registry_pb2.UnmapResponse(status=0)
            else:
                # Retorna -1 se o serviço não foi encontrado.
                return registry_pb2.UnmapResponse(status=-1)

    def Query(self, request, context):
        """Implementação do RPC Query. Consulta a localização de um serviço."""
        with self.lock:
            # Usa o método .get() do dicionário para buscar o localizador.
            locator = self.registry.get(request.service_name, "")
        return registry_pb2.QueryResponse(portmapper_locator=locator)

    def Finalize(self, request, context):
        """Implementação do RPC Finalize para shutdown gracioso."""
        with self.lock:
            count = len(self.registry)
        
        # Função que para o servidor.
        def shutdown():
            # Espera um curto período para garantir que a resposta RPC seja enviada.
            time.sleep(0.5)
            self.server.stop(1)

        # Dispara a função de shutdown em uma nova thread para não bloquear.
        threading.Thread(target=shutdown).start()
        return registry_pb2.FinalizeResponse(registered_services_count=count)

def serve():
    """Função principal que configura e inicia o servidor gRPC."""
    # O servidor Registry espera apenas um argumento: a porta em que vai rodar.
    if len(sys.argv) != 2:
        print(f"Uso: python -m {__package__}.registry_server <porto>")
        sys.exit(1)

    port = sys.argv[1]
    
    # Cria a instância do servidor com um pool de 10 threads.
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    
    # Instancia a classe de serviço e liga ao servidor gRPC.
    registry_pb2_grpc.add_RegistryServicer_to_server(
        RegistryServicer(server), server
    )
    
    # Define a porta em que o servidor irá escutar.
    server.add_insecure_port(f'[::]:{port}')
    # Inicia o servidor.
    server.start()
    #print(f"Servidor Registry iniciado na porta {port}.")
    
    # Bloqueia a thread principal, aguardando o encerramento do servidor via RPC.
    server.wait_for_termination()
    #print("Servidor Registry encerrado.")

if __name__ == '__main__':
    serve()