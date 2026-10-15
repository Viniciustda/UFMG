import os
import time
import json
import redis
import importlib.util
import sys
import zipfile
import shutil

# --- Configurações via Variáveis de Ambiente ---
REDIS_HOST = os.getenv("REDIS_HOST", "localhost")
REDIS_PORT = int(os.getenv("REDIS_PORT", 6379))
REDIS_INPUT_KEY = os.getenv("REDIS_INPUT_KEY", "metrics")
REDIS_OUTPUT_KEY = os.getenv("REDIS_OUTPUT_KEY", "output")
# Período de monitoramento configurável
MONITORING_PERIOD = float(os.getenv("MONITORING_PERIOD", "5.0"))
# Nome da função de entrada configurável (padrão: handler)
HANDLER_NAME = os.getenv("HANDLER_NAME", "handler")
# Caminho para arquivo ZIP (opcional)
ZIP_FILE_PATH = os.getenv("ZIP_FILE_PATH", "")

# Caminho padrão para módulo simples (Task 1)
SIMPLE_MODULE_PATH = "/opt/usermodule.py"

class Context:
    """Objeto de contexto"""
    def __init__(self, r_client):
        self.host = REDIS_HOST
        self.port = REDIS_PORT
        self.input_key = REDIS_INPUT_KEY
        self.output_key = REDIS_OUTPUT_KEY
        self.function_getmtime = time.time()
        self.last_execution = None
        self.env = {} 

def load_user_function():
    """Carrega a função do usuário (de arquivo .py ou .zip)."""
    module = None
    
    # Tenta carregar de ZIP se especificado
    if ZIP_FILE_PATH and os.path.exists(ZIP_FILE_PATH):
        print(f"Carregando código de ZIP: {ZIP_FILE_PATH}")
        extract_path = "/tmp/user_function"
        if os.path.exists(extract_path):
            shutil.rmtree(extract_path)
        with zipfile.ZipFile(ZIP_FILE_PATH, 'r') as zip_ref:
            zip_ref.extractall(extract_path)
        
        sys.path.append(extract_path)
        try:
            spec = importlib.util.spec_from_file_location("user_module", f"{extract_path}/main.py")
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
        except Exception as e:
            print(f"Erro ao carregar main.py do ZIP: {e}")
            return None

    # Se não for ZIP, tenta carregar o arquivo simples (Task 1)
    elif os.path.exists(SIMPLE_MODULE_PATH):
        try:
            spec = importlib.util.spec_from_file_location("user_module", SIMPLE_MODULE_PATH)
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
        except Exception as e:
            print(f"Erro ao carregar módulo simples: {e}")
            return None
            
    else:
        print("Nenhum código de usuário encontrado (/opt/usermodule.py ou ZIP).")
        return None

    # Retorna a função específica configurada no ambiente
    return getattr(module, HANDLER_NAME, None)

def main():
    print("Iniciando Runtime Serverless")
    
    # Conexão com Redis
    try:
        r = redis.Redis(host=REDIS_HOST, port=REDIS_PORT, decode_responses=True)
        r.ping()
        print(f"Conectado ao Redis em {REDIS_HOST}:{REDIS_PORT}")
    except Exception as e:
        print(f"Falha ao conectar no Redis: {e}")
        sys.exit(1)

    context = Context(r)
    last_processed_data = None

    while True:
        try:
            # Ler dados do Redis
            raw_data = r.get(REDIS_INPUT_KEY)
            
            # Só processa se houver dados e eles forem novos 
            if raw_data and raw_data != last_processed_data:
                input_data = json.loads(raw_data)
                
                # Carregar função do usuário (Hot-reload simples)
                user_func = load_user_function()
                
                if user_func:
                    # Executar função
                    try:
                        result = user_func(input_data, context)
                        
                        # Salvar resultado
                        if result:
                            r.set(REDIS_OUTPUT_KEY, json.dumps(result))
                            context.last_execution = time.time()
                    except Exception as e:
                        print(f"Erro na execução da função: {e}")
                
                last_processed_data = raw_data
            
        except Exception as e:
            print(f"Erro no loop principal: {e}")
            
        # Espera o tempo configurado (Task 3)
        time.sleep(MONITORING_PERIOD)

if __name__ == "__main__":
    main()