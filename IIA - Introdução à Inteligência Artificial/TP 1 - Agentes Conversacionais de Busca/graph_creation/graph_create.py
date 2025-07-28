import osmnx as ox
import os
import pandas as pd
import argparse
import re

def create_graph(place="Belo Horizonte, MG, Brazil", place_key="amenity", place_type="university", useful_tags=["amenity", "name"]):

    # Caminho absoluto para a pasta raiz/data
    root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    output_path = os.path.join(root_dir, 'data', 'grafo.txt')

    # Caminho para o arquivo do grafo salvo
    parts = re.split(r'\s*[,|-]\s*', place)
    parts = parts[0].replace(' ', '').lower()
    graph_path = os.path.join(root_dir, 'data', f"{parts}.graphml")

    # Verifica se o grafo já foi salvo
    if os.path.exists(graph_path):
        G = ox.load_graphml(graph_path)
    else:
        # Carrega grafo de caminhada de BH
        G = ox.graph.graph_from_place(place, network_type="walk", custom_filter='["highway"~"primary|secundary|tertiary"]')
        ox.save_graphml(G, graph_path)

    # Coleta as features do tipo especificado
    features = ox.features.features_from_place(place, {place_key: place_type})

    # Filtra apenas features que possuem nome
    features = features[features["name"].notna()]
    feature_points = features.representative_point()

    # Encontra os nós do grafo mais próximos
    nn = ox.distance.nearest_nodes(G, feature_points.x, feature_points.y)

    # Atualiza os nós do grafo com metadados das features
    for node, feature in zip(nn, features[useful_tags].to_dict(orient="records")):
        feature = {k: v for k, v in feature.items() if pd.notna(v)}
        G.nodes[node].update({place_type : feature})

    # Criar dicionário de nós com suas informações
    nodes = {}
    for node_id, data in G.nodes(data=True):
        name = ""
        node_type = ""
        
        # Verifica se o nó possui o 'place_type' e 'name' antes de adicioná-lo
        if place_type in data and "name" in data[place_type]:
            name = data[place_type]["name"]
            node_type = place_type
        
        # Adiciona o nó ao dicionário apenas se ele possui 'name' e 'type'
        if name and node_type:
            node_info = {
                "x": data.get("x"),
                "y": data.get("y"),
                "name": name,
                "type": node_type
            }
            nodes[node_id] = node_info

    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    # Exporta para TXT
    with open(output_path, "w", encoding="utf-8") as file:
        file.write("# NODES\n")
        file.write("id x y name type\n")
        for node_id, data in G.nodes(data=True):
            x = data.get("x")
            y = data.get("y")
            name = ""
            node_type = ""

            if place_type in data:
                name = data[place_type].get("name", "")
                node_type = place_type
            
            # Escapa aspas no nome, se houver
            name = name.replace('"', "'")
            file.write(f'{node_id} {x} {y} "{name}" "{node_type}"\n')

        file.write("\n# EDGES\n")
        file.write("from_id to_id length\n")
        for u, v, data in G.edges(data=True):
            if "length" not in data:
                continue
            length = data["length"]
            file.write(f"{u} {v} {length}\n")
    
    return G, nodes

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Cria o grafo de uma cidade com POIs específicos.")
    parser.add_argument("--place", type=str, default="Belo Horizonte, MG, Brazil")
    parser.add_argument("--place_key", type=str, default="amenity")
    parser.add_argument("--place_type", type=str, default="university")
    parser.add_argument("--tags", nargs="+", default=["amenity", "name"])
    args = parser.parse_args()

    create_graph(args.place, args.place_key, args.place_type, args.tags)