import json

def handler(input_data: dict, context: object) -> dict:
    """
    Função serverless para calcular métricas de uso de recursos.
    """
    
    # --- Calcula % de Tráfego de Saída (Egress) ---
    # Fórmula: bytes_sent / (bytes_sent + bytes_recv)
    
    bytes_sent = input_data.get('net_io_counters_eth0-bytes_sent', 0)
    bytes_recv = input_data.get('net_io_counters_eth0-bytes_recv', 0)
    total_traffic = bytes_sent + bytes_recv
    
    if total_traffic > 0:
        percent_network_egress = bytes_sent / total_traffic
    else:
        percent_network_egress = 0.0

    # --- Calcula % de Memória em Cache ---
    # Fórmula: (buffers + cached) / total
    
    mem_total = input_data.get('virtual_memory-total', 1) # Evita divisão por zero
    mem_buffers = input_data.get('virtual_memory-buffers', 0)
    mem_cached = input_data.get('virtual_memory-cached', 0)
    
    percent_memory_caching = (mem_buffers + mem_cached) / mem_total

    # --- Calcula Média Móvel da CPU (últimos 60s) ---
    # Dados chegam a cada 5s. Para 60s, precisa de uma janela de 12 amostras.
    # Usa context.env para persistir essa lista entre chamadas.
    
    # Inicializa a estrutura no env se não existir
    if "cpu_history" not in context.env:
        context.env["cpu_history"] = {}

    cpu_avgs = {}
    
    # Identifica quantas CPUs existem baseadas nas chaves de entrada
    # O input tem chaves como 'cpu_percent-0', 'cpu_percent-1', etc.
    for key, value in input_data.items():
        if key.startswith("cpu_percent-"):
            cpu_id = key.split("-")[1] # Pega o número da CPU (ex: '0')
            
            # Recupera o histórico dessa CPU específica
            history = context.env["cpu_history"].get(cpu_id, [])
            
            # Adiciona o valor atual
            history.append(value)
            
            # Mantém apenas os últimos 12 valores (60 segundos / 5 segundos = 12)
            if len(history) > 12:
                history.pop(0)
            
            # Salva de volta no env para a próxima execução
            context.env["cpu_history"][cpu_id] = history
            
            # Calcula a média atual
            avg_val = sum(history) / len(history)
            cpu_avgs[f"avg-util-cpu{cpu_id}-60sec"] = avg_val

    # --- Monta o JSON de Resposta ---
    output = {
        "percent-network-egress": percent_network_egress,
        "percent-memory-caching": percent_memory_caching
    }
    
    # Adiciona as médias das CPUs ao output
    output.update(cpu_avgs)
    
    return output