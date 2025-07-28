# portmapper_server.py

# Módulos padrão do Python
import grpc                     
from concurrent import futures  
import sys                      
import threading                
import time                     
import socket                   

# Módulos gerados pelo gRPC a partir dos arquivos .proto
from . import portmapper_pb2
from . import portmapper_pb2_grpc
from registry import registry_pb2, registry_pb2_grpc


class PortMapperServicer(portmapper_pb2_grpc.PortMapperServicer):
    """
    Esta classe implementa a lógica do serviço PortMapper.
    Ela herda da classe base gerada pelo gRPC e implementa os métodos RPC definidos no .proto.
    """

    def __init__(self, server_instance, own_port, registry_address=None):
        """
        Construtor do Servicer. Inicializa o estado do servidor.
        :param server_instance: A referência ao objeto principal do servidor gRPC.
        :param own_port: A porta na qual este Portmapper está executando.
        :param registry_address: O endereço do Registry, se houver.
        """
        # --- Estado do Servidor ---
        # Dicionário principal: mapeia nome do serviço -> (porto, valor)
        self.services = {}
        # Dicionário de busca reversa para remoção eficiente: mapeia porto -> nome do serviço
        self.port_to_service = {}
        # Contador para alocação de novos portos. Começa em 1025.
        self.next_port = 1025
        # Lock para garantir que o acesso aos dicionários seja seguro.
        # O gRPC pode atender múltiplas requisições em paralelo.
        self.lock = threading.Lock()

        # --- Configuração ---
        # Referência ao servidor gRPC, usada para o shutdown.
        self.server = server_instance
        # Porta e endereço do Registry, usados para o modo da "Etapa 2".
        self.own_port = own_port
        self.registry_address = registry_address
        # Descobre o próprio nome de domínio para se anunciar ao Registry.
        self.own_hostname = socket.getfqdn()

    def _get_own_locator(self):
        """Método auxiliar que monta o string localizador deste próprio Portmapper."""
        return f"{self.own_hostname}:{self.own_port}"

    def _notify_registry(self, action, service_name):
        """
        Método auxiliar que atua como um cliente gRPC para notificar o Registry.
        :param action: A ação a ser realizada ('map' ou 'unmap').
        :param service_name: O nome do serviço envolvido.
        """
        # Se o endereço do Registry não foi fornecido, o servidor está em modo Etapa 1.
        if not self.registry_address:
            return

        try:
            # Cria um canal de comunicação temporário com o Registry.
            with grpc.insecure_channel(self.registry_address) as channel:
                # Cria um stub para fazer a chamada RPC.
                stub = registry_pb2_grpc.RegistryStub(channel)
                if action == 'map':
                    # Chama o método Map do Registry.
                    stub.Map(registry_pb2.MapRequest(
                        service_name=service_name,
                        portmapper_locator=self._get_own_locator()
                    ))
                elif action == 'unmap':
                    # Chama o método Unmap do Registry.
                    stub.Unmap(registry_pb2.UnmapRequest(service_name=service_name))
        except grpc.RpcError:
            # Se o Registry estiver offline, apenas imprime um aviso sem quebrar o Portmapper.
            print(f"Aviso: Falha ao contatar o registry em {self.registry_address}", file=sys.stderr)


    def RegisterService(self, request, context):
        """Implementação do RPC RegisterService."""
        # Garante acesso exclusivo aos dicionários compartilhados.
        with self.lock:
            service_name = request.service_name
            value = request.value
            is_new = service_name not in self.services

            if is_new:
                # Se for um serviço novo, aloca uma nova porta.
                port = self.next_port
                self.next_port += 1
                self.port_to_service[port] = service_name
            else:
                # Se o serviço já existe, apenas obtém o porto já alocado.
                port = self.services[service_name][0]
            
            # Atualiza (ou insere) a entrada do serviço.
            self.services[service_name] = (port, value)

        # A notificação ao Registry é feita fora do lock. Para evitar segurar o lock durante uma chamada de rede, que pode ser lenta e assim não bloqueia outras requisições que cheguem ao Portmapper.
        self._notify_registry('map', service_name)

        return portmapper_pb2.RegisterResponse(port=port)


    def UnregisterService(self, request, context):
        """Implementação do RPC UnregisterService."""
        service_name_to_unmap = None
        status = -1
        
        with self.lock:
            port_to_remove = request.port
            if port_to_remove in self.port_to_service:
                # Usa o dicionário de busca reversa para encontrar o nome do serviço.
                service_name_to_unmap = self.port_to_service.pop(port_to_remove)
                # Remove o serviço do dicionário principal.
                self.services.pop(service_name_to_unmap)
                status = 0
        
        # Se um serviço foi removido, notifica o Registry.
        if service_name_to_unmap:
            self._notify_registry('unmap', service_name_to_unmap)

        return portmapper_pb2.UnregisterResponse(status=status)

    def GetServiceInfo(self, request, context):
        """Implementação do RPC GetServiceInfo."""
        with self.lock:
            service_name = request.service_name
            port, value = self.services.get(service_name, (-1, 0.0))
            return portmapper_pb2.ServiceInfoResponse(port=port, value=value)

    def Terminate(self, request, context):
        """Implementação do RPC Terminate para shutdown."""
        with self.lock:
            count = len(self.services)

        # Função que para o servidor.
        def shutdown():
            # Espera um curto período para garantir que a resposta RPC seja enviada antes que o servidor pare completamente.
            time.sleep(0.5)
            self.server.stop(1)

        # Dispara a função de shutdown em uma nova thread para não bloquear o retorno da chamada.
        threading.Thread(target=shutdown).start()
        return portmapper_pb2.TerminateResponse(count=count)

def serve():
    """Função principal que configura e inicia o servidor gRPC."""
    # Validação dos argumentos da linha de comando.
    if not (2 <= len(sys.argv) <= 3):
        print(f"Uso: python -m {__package__}.portmapper_server <porto> [localizador_do_registry]")
        sys.exit(1)

    # Determina se está no modo Etapa 1 ou 2 com base no número de argumentos.
    own_port = sys.argv[1]
    registry_locator = sys.argv[2] if len(sys.argv) == 3 else None
    
    # Cria a instância do servidor com um pool de 10 threads para atender requisições.
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    
    # Instancia a classe de serviço e liga ao servidor gRPC.
    portmapper_pb2_grpc.add_PortMapperServicer_to_server(
        PortMapperServicer(server, own_port, registry_locator), server
    )

    # Define a porta em que o servidor irá escutar. '[::]' significa todas as interfaces de rede.
    server.add_insecure_port(f'[::]:{own_port}')
    # Inicia o servidor (não bloqueante).
    server.start()
    #print(f"Servidor Portmapper iniciado na porta {own_port}.")
    
    # Bloqueia a thread principal, aguardando o encerramento do servidor.
    # O servidor só vai parar quando server.stop() for chamado (pelo método Terminate).
    server.wait_for_termination()
    #print("Servidor Portmapper encerrado.")

# Bloco padrão que executa a função serve().
if __name__ == '__main__':
    serve()