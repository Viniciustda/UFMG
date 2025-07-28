import os
import re
import sys
import imageio
import platform
import subprocess
from PIL import Image
import networkx as nx
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Patch
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from graph_creation.graph_create import create_graph

# Variáveis globais para armazenar o último resultado de busca
last_path = []
last_distance = 0.0

# Armazenamento dos dados globais para reutilização no agente
graph_data = {
    "G": None,
    "nodes": {},
    "valid_nodes": {},
    "place": "Belo Horizonte, MG, Brazil",
    "place_key": "amenity",
    "place_type": "university",
    "tags": ["amenity", "name"],
    "name_to_id": {},
    "id_to_name": {},
    "image_path": "../data/route.png",
    "gif_path": "../data/route_gif.png"
}

def initialize_graph(place=None, place_key=None, place_type=None, tags=None):
    """
    Cria ou carrega o grafo da cidade com base nos parâmetros fornecidos.
    Se nenhum parâmetro for fornecido, usa universidades em BH como padrão.
    """
    graph_data["place"] = place or graph_data["place"]
    graph_data["place_key"] = place_key or graph_data["place_key"]
    graph_data["place_type"] = place_type or graph_data["place_type"]
    graph_data["tags"] = tags or graph_data["tags"]

    G, nodes = create_graph(
        place=graph_data["place"],
        place_key=graph_data["place_key"],
        place_type=graph_data["place_type"],
        useful_tags=graph_data["tags"]
    )

    graph_data["G"] = G
    graph_data["nodes"] = nodes
    graph_data["valid_nodes"] = sorted([data["name"] for data in nodes.values() if data.get("name")])
    graph_data["name_to_id"] = {data["name"].lower(): node_id for node_id, data in nodes.items()}
    graph_data["id_to_name"] = {node_id: data["name"] for node_id, data in nodes.items()}

def get_id_by_name(name):
    """Retorna o ID do nó a partir do nome (ignorando maiúsculas/minúsculas)."""
    return graph_data["name_to_id"].get(name.lower())

def get_name_by_id(node_id):
    """Retorna o nome do nó a partir do ID."""
    return graph_data["id_to_name"].get(node_id)

def list_graph_nodes():
    """
    Retorna uma lista de nós que possuem nome no grafo carregado.
    Cada item da lista é uma string com o nome do nó.
    """
    return graph_data["valid_nodes"]

def compile_cpp_code(debug=False):
    """
    Executa o comando 'make' para compilar o código C++.
    Retorna True se a compilação foi bem-sucedida, False caso contrário.
    """
    if debug:
        print("[DEBUG - INFO] Compilando o código C++ com make...")
    try:
        result = subprocess.run(["make"], cwd=os.path.abspath(os.path.join(os.path.dirname(__file__), "..")), capture_output=True, text=True)
        if result.returncode == 0:
            if debug:
                print("[DEBUG - SUCESSO] Compilação concluída.")
            return True
        else:
            print("[ERRO] Erro na compilação:")
            print(result.stderr)
            return False
    except Exception as e:
        print(f"[EXCEÇÃO] Falha ao executar make: {e}")
        return False
    
def run_search(start_name, end_name, algorithm="astar", exec_path="../build/tp1.exe", graph_path="../data/grafo.txt", result_path="../data/search_result.txt", debug=False):
    """
    Executa o binário C++ com os dados de entrada.
    
    Parâmetros:
    - start_name (str): Nome do nó de origem.
    - end_name (str): Nome do nó de destino.
    - algorithm (str): 'dijkstra' ou 'astar'.
    - exec_path (str): Caminho para o executável C++.
    - result_path (str): Caminho para o arquivo com os resultados da busca.
    """
    global last_path, last_distance

    if not os.path.exists(exec_path):
        raise FileNotFoundError(f"Executável não encontrado em {exec_path}")

    # Procurar IDs dos nós com base no nome
    name_to_id = graph_data.get("name_to_id", {})
    start_id = name_to_id.get(start_name.lower())
    end_id = name_to_id.get(end_name.lower())

    if start_id is None:
        return f"\n{start_name} esta fora de alcance.\n"
    if end_id is None:
        return f"\n{end_name} esta fora de alcance.\n"

    # Executar o binário com entrada via stdin
    process = subprocess.Popen([exec_path, graph_path], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    input_data = f"{start_id}\n{end_id}\n{algorithm}\n"
    stdout, stderr = process.communicate(input=input_data)

    if process.returncode != 0:
        raise RuntimeError(f"Erro ao executar algoritmo de busca:\n{stderr}")

    # Verifica se o arquivo de resultado foi criado
    if not os.path.exists(result_path):
        raise FileNotFoundError(f"Arquivo de resultado '{result_path}' não encontrado.")

    # Lê o resultado
    with open(result_path, "r", encoding="utf-8") as file:
        lines = [line.strip() for line in file if line.strip()]
    
    if not lines or lines[0] == "-1":
        return "\nCaminho nao encontrado.\n"

    distance = float(lines[0])
    path = []
    for line in lines[1:]:
        match = re.match(r'^(\d+)\s+([-\d.]+)\s+([-\d.]+)\s+"(.*?)"\s+"(.*?)"$', line)
        if match:
            node_id = int(match.group(1))
            x = float(match.group(2))
            y = float(match.group(3))
            name = match.group(4)
            place_type = match.group(5)
            path.append({
                "id": node_id,
                "x": x,
                "y": y,
                "name": name,
                "type": place_type
            })

    # Atualiza o estado global
    last_path = path
    last_distance = distance

    # Gera descrição automaticamente
    description = describe_route()
    return f"\nRota encontrada com sucesso!\n\n{description}"

def describe_route():
    """
    Gera uma descrição textual da rota, com nomes dos locais (se disponíveis) e IDs dos nós.
    """
    if not last_path:
        return "Nao foi possível encontrar uma rota entre os pontos fornecidos."

    steps = []
    for node in last_path:
        name = node["name"].strip()
        if name:
            passo = f"no {node['id']} que e o(a) {name}"
        else:
            passo = f"no {node['id']}"
        steps.append(passo)

    if len(steps) == 1:
        return f"O ponto de origem e destino e o mesmo: {steps[0]}."

    origem = steps[0]
    destino = steps[-1]
    meio = ",\ndepois para o ".join(steps[1:-1])

    if meio:
        return f"Saindo do {origem}, \nvamos para o {meio} \ne por fim chegamos ao {destino}. \nCom uma distancia total de {last_distance:.2f} metros.\n"
    else:
        return f"Saindo do {origem}, chegamos diretamente ao {destino}. Com uma distancia total de {last_distance:.2f} metros.\n"

def generate_route_image(output_path="../data/route.png"):
    """
    Gera uma imagem do grafo com a rota destacada.

    Parâmetros:
    - distance (float): Distância total do caminho.
    - path (list): Lista de nós (dicts) com 'id', 'x', 'y', 'name' e 'type'.
    - output_path (str): Caminho para salvar a imagem.
    """
    # Acessa o grafo completo
    G = graph_data["G"]
    
    # Garante que a pasta de saída exista
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    # Obtém as posições dos nós (x, y) para todos os nós
    pos = {node_id: (data["x"], data["y"]) for node_id, data in G.nodes(data=True)}

    # Cria uma figura para o gráfico
    fig, ax = plt.subplots(figsize=(12, 10))

    # Desenha o grafo completo
    nx.draw(G, pos, with_labels=False, node_size=50, node_color="lightgray", font_size=8, font_color="black", edge_color="gray")

    # Plotando o caminho encontrado em destaque
    route_x = [node["x"] for node in last_path]
    route_y = [node["y"] for node in last_path]

    # Desenha o caminho com a cor azul
    plt.plot(route_x, route_y, 'o-', color='blue', linewidth=2, markersize=6, label='Rota')

    # Destacar origem e destino
    plt.plot(route_x[0], route_y[0], 'go', markersize=10, label='Origem')
    plt.plot(route_x[-1], route_y[-1], 'ro', markersize=10, label='Destino')

    # Adiciona o nome dos locais no gráfico
    names = [node["name"] for node in last_path]
    for x, y, name in zip(route_x, route_y, names):
        plt.text(x, y, name, fontsize=8, ha='right', va='bottom')

    # título e layout
    ax.set_title(f"Rota encontrada - Distância: {last_distance:.2f} metros")
    ax.set_xlabel("Longitude")
    ax.set_ylabel("Latitude")
    ax.legend()
    ax.grid(True)

    plt.tight_layout()
    fig.subplots_adjust(top=0.92)

    fig.savefig(output_path, dpi=300)
    plt.close(fig)

    graph_data["image_path"] = output_path

    return output_path

def open_route_image():
    """
    Abre a imagem gerada da rota.
    """
    try:
        # Abre a imagem usando Pillow
        img = Image.open(graph_data["image_path"])

        # Exibe a imagem
        img.show()

    except FileNotFoundError:
        print(f"[Erro] Arquivo de imagem não encontrado: {graph_data['image_path']}")
    except Exception as e:
        print(f"[Erro] Ocorreu um erro ao abrir a imagem: {e}")

def return_image_path():
    """
    Retorna o caminho da imagem gerada.
    """
    return graph_data["image_path"]

def generate_route_gif(output_path="../data/route_gif.png", duration=0.5):
    """
    Gera um GIF animado da rota sendo traçada sobre o grafo.
    
    Parâmetros:
    - output_path (str): Caminho para salvar o GIF.
    - duration (float): Tempo de cada frame em segundos.
    """
    if not last_path:
        print("[Info] Nenhum caminho fornecido para animação.")
        return

    G = graph_data["G"]
    pos = {node: (G.nodes[node]['x'], G.nodes[node]['y']) for node in G.nodes}
    
    # Criar pasta temporária
    temp_folder = "../data/temp_frames"
    os.makedirs(temp_folder, exist_ok=True)
    filenames = []

    route_nodes = [node["id"] for node in last_path]

    for i in range(1, len(route_nodes) + 1):
        fig, ax = plt.subplots(figsize=(12, 12))

        # Desenha o grafo completo
        nx.draw(G, pos=pos, node_size=10, node_color="lightgray", edge_color="gray", ax=ax)

        # Separa origem, destino e intermediários (parciais até o passo i)
        current_nodes = route_nodes[:i]
        origin = [route_nodes[0]]
        destination = [route_nodes[-1]]
        intermediates = [n for n in current_nodes if n not in origin + destination]

        # Desenha os nós com cores distintas
        nx.draw_networkx_nodes(G, pos, nodelist=intermediates, node_size=50, node_color='darkblue', ax=ax)
        nx.draw_networkx_nodes(G, pos, nodelist=origin, node_size=100, node_color='green', ax=ax)
        if i == len(route_nodes):  # só desenha destino no último frame
            nx.draw_networkx_nodes(G, pos, nodelist=destination, node_size=100, node_color='red', ax=ax)

        # Desenha as arestas do caminho até o nó i
        if i > 1:
            nx.draw_networkx_edges(
                G, pos, edgelist=list(zip(route_nodes[:i - 1], route_nodes[1:i])), width=2, edge_color='blue', ax=ax
            )

        # Mostra os nomes dos nós no caminho até i
        for node_id in current_nodes:
            node_data = G.nodes[node_id]
            name = node_data.get("name", "")
            x, y = pos[node_id]
            ax.text(x, y, name, fontsize=8, ha='right', va='bottom', color='black')

        # Título com distância
        ax.set_title(f"Distância total: {last_distance:.2f} metros", fontsize=14)

        # Legenda
        legend_elements = [
            Patch(facecolor='green', edgecolor='black', label='Origem'),
            Patch(facecolor='red', edgecolor='black', label='Destino'),
            Patch(facecolor='darkblue', edgecolor='black', label='Rota'),
            Patch(facecolor='lightgray', edgecolor='black', label='Outros nós'),
        ]
        ax.legend(handles=legend_elements, loc='upper right', fontsize=10)

        # Salva frame
        filename = os.path.join(temp_folder, f"frame_{i:03d}.png")
        plt.savefig(filename)
        plt.close()
        filenames.append(filename)

    # Criar GIF
    images = [imageio.v2.imread(fname) for fname in filenames]
    imageio.mimsave(output_path, images, duration=duration)
    print(f"[Info] GIF gerado em {output_path}")

    # Limpar arquivos temporários
    for fname in filenames:
        os.remove(fname)
    os.rmdir(temp_folder)

    graph_data["gif_path"] = output_path

    return output_path

def open_route_gif():
    """
    Abre o GIF da rota com o visualizador de imagens padrão do sistema.
    """
    gif_path = graph_data["gif_path"]

    if not os.path.exists(gif_path):
        print(f"[Erro] Arquivo {gif_path} não encontrado.")
        return

    system = platform.system()
    try:
        if system == "Darwin":  # macOS
            subprocess.run(["open", gif_path])
        elif system == "Windows":
            os.startfile(gif_path)
        else:  # Linux
            subprocess.run(["xdg-open", gif_path])
    except Exception as e:
        print(f"[Erro] Não foi possível abrir o GIF: {e}")

def return_gif_path():
    """
    Retorna o caminho do GIF gerado.
    """
    return graph_data["gif_path"]