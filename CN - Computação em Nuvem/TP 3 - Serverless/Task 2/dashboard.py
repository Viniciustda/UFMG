import streamlit as st
import redis
import json
import time
import os
import pandas as pd

# Se estiver rodando no Kubernetes, pegamos do ENV. Se for local, usa localhost.
redis_host = os.getenv('REDIS_HOST', 'localhost')
redis_port = int(os.getenv('REDIS_PORT', 6379))
redis_key = os.getenv('REDIS_OUTPUT_KEY', 'viniciusabel-proj3-output') 

# Conexão com o Redis
@st.cache_resource
def get_redis_client():
    return redis.Redis(host=redis_host, port=redis_port, decode_responses=True)

st.title("Monitoramento Serverless - TP3")
st.write(f"Lendo dados do Redis: **{redis_host}:{redis_port}** | Key: **{redis_key}**")

# Container para os dados atualizarem sem recarregar a página toda
placeholder = st.empty()

# Loop de atualização
while True:
    try:
        r = get_redis_client()
        data_json = r.get(redis_key)
        
        if data_json:
            data = json.loads(data_json)
            
            with placeholder.container():
                # Métricas Principais (KPIs)
                col1, col2 = st.columns(2)
                col1.metric("Tráfego de Saída (Egress)", f"{data.get('percent-network-egress', 0):.2%}")
                col2.metric("Memória em Cache", f"{data.get('percent-memory-caching', 0):.2%}")
                
                # Gráfico de CPUs
                st.subheader("Utilização Média das CPUs (60s)")
                
                # Prepara dados para o gráfico filtrando apenas as chaves de CPU
                cpu_data = {k: v for k, v in data.items() if 'avg-util-cpu' in k}
                
                if cpu_data:
                    # Cria um DataFrame simples para o gráfico
                    df_cpu = pd.DataFrame(list(cpu_data.items()), columns=['CPU', 'Utilização (%)'])
                    # Limpa os nomes para o gráfico (avg-util-cpu0-60sec -> cpu0)
                    df_cpu['CPU'] = df_cpu['CPU'].str.replace('avg-util-', '').str.replace('-60sec', '')
                    # Ordena pelo nome da CPU
                    df_cpu = df_cpu.sort_values('CPU')
                    
                    st.bar_chart(df_cpu.set_index('CPU'))
                else:
                    st.warning("Aguardando dados de CPU...")
                    
                st.caption(f"Última atualização: {time.strftime('%H:%M:%S')}")
        else:
            with placeholder.container():
                st.warning(f"Chave '{redis_key}' não encontrada no Redis.")

    except Exception as e:
        with placeholder.container():
            st.error(f"Erro ao ler/decodificar dados: {e}")
            
    # Aguarda 2 segundos antes de atualizar
    time.sleep(2)