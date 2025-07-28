from smolagents import CodeAgent, LiteLLMModel, tool
import agent_tools as agents_tools
import gradio as gr

# === Ferramentas para o agente usar ===

@tool
def list_nodes_tool() -> str:
    """
    This is a tool that lists the universities in Belo Horizonte available in the graph.
    It returns a string with the names of the universities in Belo Horizonte.
    Args:
        None (It does not take any arguments).
    """
    nodes = agents_tools.list_graph_nodes()
    return "\n".join(nodes)

@tool
def search_route_tool(origem: str, destino: str, algoritmo: str) -> str:
    """
    This tool finds the shortest path between two universities in Belo Horizonte using either Dijkstra or A* algorithm.
    It generates both a static image and an animated GIF of the route.
    It returns a complete description of the path, including the image and GIF file paths.

    Args:
        origem: Name of the starting point.
        destino: Name of the destination point.
        algoritmo: Search algorithm to use ("dijkstra" or "astar").
    """

    # Verifica se o algoritmo é válido
    if algoritmo in ["A*", "a*", "Astar", "ASTAR", "A STAR", "a star", "A Star", "A star", "Aestrela", "AESTRELA", "A ESTRELA", "a estrela", "A Estrela", "A estrela", "aestrela"]:
        algoritmo = "astar"
    elif algoritmo in ["Dijkstra", "Dikstra", "Dikistra", "Djikstra", "Djikistra", "dijkistra", "dikistra", "djikstra", "djikistra"]:
        algoritmo = "dijkstra"

    # Executa a busca da rota
    path_description = agents_tools.run_search(origem, destino, algoritmo)

    if path_description == "\nCaminho nao encontrado.\n" or path_description == f"\n{origem} esta fora de alcance.\n" or path_description == f"\n{destino} esta fora de alcance.\n":
        return f"{path_description}\n\n"

    # Gera imagem e GIF
    image_path = agents_tools.generate_route_image()
    gif_path = agents_tools.generate_route_gif()

    # Monta a resposta
    result = (
        f"{path_description}\n\n"
        f"📷 Imagem da rota: {image_path}\n"
        f"🎞 GIF animado da rota: {gif_path}"
    )
    return result

# === Modelo LLM via Ollama (qwen 3 local) ===

model = LiteLLMModel(
    model_id="ollama/qwen3",               # Nome do modelo conforme carregado no Ollama
    api_base="http://localhost:11434",      # Endereço padrão do Ollama
    api_key="",                             # Não precisa de chave para uso local
    num_ctx=8192                            # Contexto maior (importante para tarefas complexas)
)

# === Agente conversacional ===

agent = CodeAgent(
    name="agente_de_rotas",
    description=(
        "Você é um assistente de rotas especializado em calcular caminhos entre universidades de Belo Horizonte. "
        "Seu objetivo é responder apenas a duas coisas: (1) listar universidades disponíveis e (2) calcular a melhor rota entre duas delas usando 'astar' ou 'dijkstra'."
        "Para calcular uma rota, você deve identificar claramente a origem, o destino e o algoritmo de busca. Se o usuário não informar o algoritmo ou usar um nome inválido, assuma 'astar' como padrão."
        "Voce possui duas ferramentas: list_nodes_tool() e search_route_tool(origem, destino, algoritmo)."
        "Use sempre a ferramenta list_nodes_tool() para mostrar ou listar as universidades existentes no grafo de Belo Horizonte. Seu comportamento é o seguinte: ao identificar a intenção do usuário (listar universidades), você deve chamar a ferramenta list_nodes_tool() e IMEDIATAMENTE retornar a resposta EXATAMENTE como ela for retornada pela ferramenta, sem reescrever, resumir ou acrescentar nada. NUNCA tente explicar ou interpretar o conteúdo retornado pelas ferramentas. Considere a resposta da ferramenta como final e completa. Não continue pensando ou respondendo após chamar uma ferramenta — a execução termina ali."
        "Use sempre a ferramenta search_route_tool(origem, destino, algoritmo) para retornar o caminho entre duas universidades. Seu comportamento é o seguinte: ao identificar a intenção do usuário (calcular uma rota), você deve chamar a ferramenta search_route_tool(origem, destino, algoritmo) e IMEDIATAMENTE retornar a resposta EXATAMENTE como ela for retornada pela ferramenta, sem reescrever, resumir ou acrescentar nada. NUNCA tente explicar ou interpretar o conteúdo retornado pelas ferramentas. Considere a resposta da ferramenta como final e completa. Não continue pensando ou respondendo após chamar uma ferramenta — a execução termina ali."
        "Quando o usuario informa um nome de universidade que não existe no grafo, a ferramenta search_route_tool(origem, destino, algoritmo) retorna a string: 'universidade' esta fora de alcance. Voce deve apenas imprimir essa string se essa situacao ocorrer."
    ),
    tools=[
        list_nodes_tool,
        search_route_tool
    ],
    model=model
)

# === Inicialização padrão ===

agents_tools.initialize_graph()
agents_tools.compile_cpp_code()

# === Função de resposta para Gradio ===

def responder(user_input, chat_history):
    if user_input.strip().lower() in ["sair", "exit", "quit"]:
        chat_history.append({"role": "user", "content": user_input})
        chat_history.append({"role": "assistant", "content": "Até logo!"})
        return chat_history, "", None, None, gr.update(visible=False), gr.update(visible=False)

    resposta = agent.run(user_input)
    if hasattr(resposta, "content"):
        resposta = resposta.content

    chat_history.append({"role": "user", "content": user_input})
    chat_history.append({"role": "assistant", "content": resposta})

    # Checa se a resposta envolve imagem ou GIF
    img_visible = False
    gif_visible = False
    img_path = None
    gif_path = None

    path_image = agents_tools.return_image_path()
    path_gif = agents_tools.return_gif_path()

    if isinstance(resposta, str) and path_image in resposta:
        img_path = path_image
        img_visible = True

    if isinstance(resposta, str) and path_gif in resposta:
        gif_path = path_gif
        gif_visible = True

    return chat_history, "", img_path, gif_path, gr.update(visible=img_visible), gr.update(visible=gif_visible)

# === Interface com Gradio ===

with gr.Blocks(title="Agente de Rotas") as demo:
    gr.Markdown("## Agente de Rotas")
    gr.Markdown("""
    Olá! Sou seu assistente de rotas.  
    Atualmente, tenho um grafo com **universidades em BH**.

    Você pode:
    - Pedir uma lista com as inuversidades disponíveis para consulta: _"Quais são as universidades disponíveis?"_
    - Pedir uma rota escolhendo o algoritmo de busca (astar ou dijkstra): _"Qual o melhor caminho entre Campus Pampulha da Universidade Federal de Minas Gerais e PUC Minas - Campus Barreiro usando astar?"_
                
    A rota será descrita em texto, com o caminho e a distância total.
    Além disso, será apresentada em forma de imagem e GIF animado.

    Como posso te ajudar?
    """)

    chatbot = gr.Chatbot(label="Conversa com o Agente", type="messages")
    msg = gr.Textbox(label="Digite sua mensagem aqui:")
    state = gr.State([])

    imagem = gr.Image(label="Imagem da rota", type="filepath", visible=False)
    gif = gr.Image(label="GIF da rota", type="filepath", visible=False)

    def limpar_input():
        return ""

    msg.submit(responder, [msg, state], [chatbot, msg, imagem, gif, imagem, gif])
    msg.submit(limpar_input, None, msg)

# === Rodar a interface ===

if __name__ == "__main__":
    demo.launch(allowed_paths=["../data"])