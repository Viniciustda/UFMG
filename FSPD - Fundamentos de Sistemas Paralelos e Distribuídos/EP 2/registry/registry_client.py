# registry_client.py

# Módulos padrão do Python
import grpc
import sys

# Imports dos módulos gerados pelo gRPC.
# Note que este cliente precisa conhecer AMBOS os serviços, pois ele conversa com o Registry e, em seguida, com o Portmapper.
from . import registry_pb2, registry_pb2_grpc
from portmapper import portmapper_pb2, portmapper_pb2_grpc


def run():
    """
    Função principal que executa a lógica do cliente do Registry.
    """
    # O cliente espera um único argumento: o endereço do servidor Registry.
    if len(sys.argv) != 2:
        print(f"Uso: python -m {__package__}.registry_client <host_do_registry:porto>")
        sys.exit(1)

    registry_address = sys.argv[1]

    # Bloco try-except para lidar com o caso do Registry estar offline desde o início.
    try:
        # Abre um canal de comunicação com o servidor Registry. Este canal permanecerá aberto durante a execução do cliente.
        with grpc.insecure_channel(registry_address) as registry_channel:
            # Cria o stub para fazer chamadas RPC para o Registry.
            registry_stub = registry_pb2_grpc.RegistryStub(registry_channel)

            # Loop principal que lê comandos da entrada padrão (stdin).
            for line in sys.stdin:
                # O split com maxsplit=1 para o comando "Q nome do servico"
                parts = line.strip().split(maxsplit=1)
                if not parts:
                    continue
                
                command = parts[0]

                # --- Lógica para o comando 'Q' (Query) ---
                # Este é o fluxo de descoberta de serviço de 3 etapas.
                if command == 'Q' and len(parts) == 2:
                    service_name = parts[1]
                    
                    # ETAPA 1: Consultar o Registry para descobrir onde o serviço está.
                    query_response = registry_stub.Query(
                        registry_pb2.QueryRequest(service_name=service_name)
                    )
                    portmapper_locator = query_response.portmapper_locator

                    # Se o Registry retornou um localizador (não uma string vazia)...
                    if portmapper_locator:
                        # ETAPA 2: Conectar ao Portmapper específico.
                        try:
                            # Cria um novo canal, para o Portmapper no endereço retornado. Este canal é temporário e fechado após a consulta.
                            with grpc.insecure_channel(portmapper_locator) as pm_channel:
                                # Cria o stub para o serviço PortMapper.
                                pm_stub = portmapper_pb2_grpc.PortMapperStub(pm_channel)
                                
                                # ETAPA 3: Consultar o Portmapper para obter os detalhes finais.
                                info_response = pm_stub.GetServiceInfo(
                                    portmapper_pb2.ServiceInfoRequest(service_name=service_name)
                                )
                                
                                # Se o Portmapper encontrou o serviço, imprime os detalhes.
                                if info_response.port != -1:
                                    print(f"{info_response.port} {info_response.value:.6f}")

                        except grpc.RpcError:
                            # Trata o caso em que o Registry fornece um endereço, mas o Portmapper naquele endereço está offline. O cliente não quebra.
                            pass

                # --- Lógica para o comando 'F' (Finalize) ---
                elif command == 'F':
                    request = registry_pb2.FinalizeRequest()
                    # Chama o método Finalize do Registry.
                    finalize_response = registry_stub.Finalize(request)
                    print(finalize_response.registered_services_count)
                    # Encerra o cliente.
                    break
    
    except grpc.RpcError:
        print(f"Erro de comunicação com o Registry em {registry_address}. O servidor pode estar offline.", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    run()