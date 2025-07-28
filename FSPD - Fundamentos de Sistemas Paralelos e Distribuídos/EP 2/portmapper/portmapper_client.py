# portmapper_client.py

# Módulos padrão do Python
import grpc  
import sys   

# Módulos gerados pelo gRPC, importados de forma relativa.
from . import portmapper_pb2
from . import portmapper_pb2_grpc


def run():
    """
    Função principal que executa a lógica do cliente.
    """
    # O cliente espera um único argumento: o endereço do servidor no formato "host:porto".
    if len(sys.argv) != 2:
        # Mensagem de uso.
        print(f"Uso: python -m {__package__}.{__file__.split('/')[-1]} <host_do_servidor:porto>")
        sys.exit(1)

    server_address = sys.argv[1]

    # With garante que o canal de comunicação com o servidor será fechado corretamente ao final do bloco, mesmo que ocorram erros.
    try:
        with grpc.insecure_channel(server_address) as channel:
            # O 'stub' é o objeto que o cliente usa para fazer as chamadas RPC.
            # Ele possui os mesmos métodos definidos no serviço no arquivo .proto.
            stub = portmapper_pb2_grpc.PortMapperStub(channel)

            # Loop principal: lê comandos da entrada padrão (stdin), linha por linha.
            for line in sys.stdin:
                # Processa a linha de entrada, removendo espaços em branco e dividindo em, no máximo, 3 partes (comando, arg1, arg2).
                parts = line.strip().split(maxsplit=2)
                if not parts:
                    continue  # Ignora linhas em branco.
                
                command = parts[0]

                # --- Lógica para o comando 'R' (Register) ---
                if command == 'R' and len(parts) == 3:
                    service_name = parts[1]
                    try:
                        value = float(parts[2])
                        # 1. Cria o objeto de requisição (RegisterRequest) com os dados.
                        request = portmapper_pb2.RegisterRequest(service_name=service_name, value=value)
                        # 2. Chama o método remoto no servidor.
                        response = stub.RegisterService(request)
                        # 3. Imprime o resultado recebido na resposta.
                        print(response.port)
                    except ValueError:
                        # Ignora a linha se o valor não for válido.
                        continue

                # --- Lógica para o comando 'U' (Unregister) ---
                elif command == 'U' and len(parts) == 2:
                    try:
                        port = int(parts[1])
                        request = portmapper_pb2.UnregisterRequest(port=port)
                        response = stub.UnregisterService(request)
                        print(response.status)
                    except ValueError:
                        # Ignora a linha se o porto não for válido.
                        continue

                # --- Lógica para o comando 'G' (Get) ---
                elif command == 'G' and len(parts) == 2:
                    service_name = parts[1]
                    request = portmapper_pb2.ServiceInfoRequest(service_name=service_name)
                    response = stub.GetServiceInfo(request)
                    # Conforme o enunciado, só imprime algo se o serviço for encontrado (porto != -1).
                    if response.port != -1:
                        # Formata o valor com 6 casas decimais. Só para garantir a precisão.
                        print(f"{response.port} {response.value:.6f}")

                # --- Lógica para o comando 'T' (Terminate) ---
                elif command == 'T' and len(parts) == 1:
                    # Cria a mensagem de requisição vazia.
                    request = portmapper_pb2.TerminateRequest()
                    response = stub.Terminate(request)
                    print(response.count)
                    # Encerra o loop e, consequentemente, o cliente.
                    break
    
    except grpc.RpcError as e:
        # Captura erros de comunicação com o servidor.
        print(f"Erro de comunicação com o servidor em {server_address}: O servidor pode estar offline.", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    run()