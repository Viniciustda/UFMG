import plotly.graph_objects as go
import sys 
#Para leitura de arquivo por parâmetro

#Execução:

#python3 main.py <caminho do arquivo>


#Formato do Arquivo de Entrada:

# Each file consists of one line divided in two parts. The first part is an integer value that represents the number of vertices of the polygon.
# The second part is a counterclockwise sequence of the vertices. Each vertex is represented by its x and y coordinates each of which is written as the quotient of two integers int/int.
# As an example, here is the representation of a square with coordinates (1, 1) (50, 1) (50, 50) and (1, 50):
#        4   1/1   1/1   100/2   1/1   500/10   50/1   1/1   100/2
# https://www.ic.unicamp.br/~cid/Problem-instances/Art-Gallery/AGPVG/index.html


#Todas as etapas da triangulação
triangulos_plot = [] 

# Função para ler o polígono a partir do arquivo
def readPolygon():

    filename = ''

    if len(sys.argv) < 2: 
        print("Parâmetro não encontrado. Insira o caminho do arquivo:")
        filename = input()
    else:
        filename = sys.argv[1]

    with open(filename, 'r') as file:
        data = file.read().strip().split()
    
    n_vertices = int(data[0])
    #pega cada valor separado por espaço de 1 até quantidade de vértices/2(pontos)+1
    #divide pra pegar x e y, pula de 2 em 2 "n i/i=x i/i=y"
    coordenadas = [(float(int(data[i].split('/')[0])/int(data[i].split('/')[1])),float(int(data[i+1].split('/')[0])/int(data[i+1].split('/')[1]))) for i in range(1, n_vertices*2+1,2)]
    return coordenadas

# Função para realizar a triangulação do polígono usando o método do corte de orelhas
def earClippingTriangulation(polygon):
    # Função para verificar se dados 3 pontos eles formam um triângulo convexo
    def isConvex(p1, p2, p3):
        return (p2[0] - p1[0]) * (p3[1] - p1[1]) - (p2[1] - p1[1]) * (p3[0] - p1[0]) >= 0

    # Função para verificar se um ponto está dentro de um triângulo
    def inTriangle(pt, v1, v2, v3):
        b1 = not isConvex(pt,v1, v2)
        b2 = not isConvex(pt,v2, v3)
        b3 = not isConvex(pt,v3, v1)
        return ((b1 == b2) and (b2 == b3))

    triangles = []
    global triangulos_plot
    vertices = polygon[:]
    while len(vertices) > 3:
        for i in range(len(vertices)):  

            p1 = vertices[i]
            p2 = vertices[i + 1]
            p3 = vertices[i + 2]
            
            #se os 3 pontos analisados formam um triangulo convexo
            if isConvex(p1, p2, p3):
                orelha = True
                #para cada j, se ele não faz parte/está dentro do triângulo, vertice i+1 não é ponta da orelha
                for j in vertices:
                    if not (j in (p1,p2,p3)) and inTriangle(j, p1, p2, p3):
                        orelha = False
                        triangulos_plot.append((p1, p2, p3))
                        break
                
                if orelha:
                    triangles.append((p1, p2, p3))
                    triangulos_plot.append((p1, p2, p3))
                    del vertices[i + 1]
                    break
    
    #faz ultimo triangulo
    triangles.append((vertices[0], vertices[1], vertices[2]))
    triangulos_plot.append((vertices[0], vertices[1], vertices[2]))
    return triangles

#grafo não direcionado, representação por matriz de adjacências
class Grafo:
    def __init__(self, vertices):
        self.vertices = vertices
        self.grafo = [[0 for _ in range(vertices)] for _ in range (vertices)]

    def connectVertices(self, a, b):
        self.grafo[a][b] = 1
        self.grafo[b][a] = 1
    
    #checa se alguem conectado a v tem a mesma cor
    def conectadoComMesmaCor(self, vertice, array_cores, cor):
        for i in range(self.vertices):
            if self.grafo[i][vertice] == 1 and array_cores[i] == cor:
                return False
        return True

    def vertexColor(self, quantidade_cores, array_cores, vertice_atual):        
        if vertice_atual == self.vertices:
            return True

        for cor in range(1, quantidade_cores + 1):
            if self.conectadoComMesmaCor(vertice_atual, array_cores, cor):
                array_cores[vertice_atual] = cor
                if self.vertexColor(quantidade_cores, array_cores, vertice_atual + 1):
                    return True
                array_cores[vertice_atual] = 0

        return False

# Função para colorir os vértices do polígono
def vertexColorPolygon(polygon, triangles):
    #transforma em grafo para
    #garantir que o polígono e os triangulos são 1 só objeto conectado

    #criando grafo, tamanho = vertices que compoem poligono
    g = Grafo(len(polygon))

    #traduzindo conexões
    #polígono é fechado
    #0 no 1, 1 no 2, 2 no 3, 3 no 4, 4 no 5,...
    for i in range(len(polygon)-1):
        g.connectVertices(i, i+1)
    #ligação último no primeiro, n no 0
    g.connectVertices(len(polygon)-1, 0)

    #Transforma lista de tuplas em um dicionário,
    # 0 : (coordx, coordy) , 1 : (coordx, coordy)...
    dicionario_indice_vertices = {tupla_: i for i, tupla_ in enumerate(polygon)}

    #triangulo tem 3 vertices
    #transforma em conexão, v1->v2, v2->v3, v3->v1
    conexoes = []

    for triangle in triangles:
        conexoes.append((triangle[0],triangle[1]))
        conexoes.append((triangle[1],triangle[2]))
        conexoes.append((triangle[2],triangle[0]))

    for a,b in conexoes:
        g.connectVertices(dicionario_indice_vertices.get(a),dicionario_indice_vertices.get(b))

    #rodar vertex color no grafo
    array_cores = [0] * len(polygon)
    g.vertexColor(3,array_cores,0) #no escopo, array de cores é modificado, logo pode ser usado em seguida

    vertex_colors = {}
    #se vértice preto, coloração errada
    cores = ['black', 'purple', 'yellow', 'blue', 'black']

    for i, vertex in enumerate(polygon):
        vertex_colors[vertex] = cores[array_cores[i]]

    return vertex_colors

#Função para plotar o passo da Triangulação e em seguida da Coloração juntos
#processo é feito e cada frame armazenado em um array
def plotCompleto(polygon, triangles, vertex_colors):
    global triangulos_plot
    fig = go.Figure()
    
    x, y = zip(*polygon)
    fig.add_trace(go.Scatter(visible=True,
                                    x=x + (x[0],),
                                    y=y + (y[0],),
                                    mode='lines', 
                                    fill='none',
                                    line=dict(color='blue'),
                                    name='Polígono',
                                    showlegend=True))
    
    step_index = 1
    x_cumulative = ()
    y_cumulative = ()

    for triangle in triangulos_plot:
        tx, ty = zip(*triangle)

        #se é um triangulo correto
        if triangle in triangles:
            x_cumulative += tx
            y_cumulative += ty
            fig.add_trace(go.Scatter(visible=False,
                                    x=x_cumulative + (tx[0],),
                                    y=y_cumulative + (ty[0],),
                                    mode='lines', 
                                    fill='none',
                                    line=dict(color='green'),
                                    name='Polígono Triangulado',
                                    showlegend=True))
        else:
            fig.add_trace(go.Scatter(visible=False,
                                    x=x + (x[0],) + tx + (tx[0],),
                                    y=y + (y[0],) + ty + (ty[0],),
                                    mode='lines', 
                                    fill='none',
                                    line=dict(color='red',dash='dot'),
                                    name='Vértice dentro do Triangulo',
                                    showlegend=True))
        step_index = step_index + 1

        

    triangulacao_completa = step_index-1

    for vertex, color in vertex_colors.items():
        fig.add_trace(go.Scatter(visible=False,
                                 x=[vertex[0]],
                                 y=[vertex[1]],
                                 mode='markers', 
                                 marker=dict(color=color, size=20), 
                                 name=color + str(step_index),
                                 showlegend=False))
        step_index = step_index + 1
    
    steps = []
    i = 0
    while i < len(fig.data):
        pular2 = False

        step = dict(
            method="update",
            args=[{"visible": [False] * len(fig.data)},
                {"title": "Passo: " + str(i)}],
        )
        step["args"][0]["visible"][i] = True
        step["args"][0]["visible"][0] = True #Polígono inicial sempre visível
    
        if i >= triangulacao_completa: #triangulação completa sempre visivel depois de terminar
            step["args"][0]["visible"][triangulacao_completa] = True 
            for j in range(triangulacao_completa+1,i): #deixa cada vértice colorido visível ao mesmo tempo
                step["args"][0]["visible"][j] = True

        steps.append(step)
        i += 1

    sliders = [dict(
        active=0,
        currentvalue={"prefix": ""},
        steps=steps
    )]

    fig.update_layout(title='Triangulação por Corte de Orelhas e Coloração dos vértices', 
                      xaxis_title='X', 
                      yaxis_title='Y',
                      sliders=sliders)
    
    fig.show()

#Plota somente o polígono
def plotPolygon(polygon):
    fig = go.Figure()
    
    x, y = zip(*polygon)
    fig.add_trace(go.Scatter(visible=True, x=x + (x[0],), y=y + (y[0],), mode='lines', name='Polígono'))

    fig.update_layout(title='Polígono', 
                      xaxis_title='X', 
                      yaxis_title='Y',)
    
    fig.show()

#Plota somente a triangulação
def plotTriangulacao(polygon, triangles):
    global triangulos_plot
    fig = go.Figure()
    
    x, y = zip(*polygon)
    fig.add_trace(go.Scatter(visible=True,
                                    x=x + (x[0],),
                                    y=y + (y[0],),
                                    mode='lines', 
                                    fill='none',
                                    line=dict(color='blue'),
                                    name='Polígono',
                                    showlegend=True))
    
    step_index = 1
    x_cumulative = ()
    y_cumulative = ()

    for triangle in triangulos_plot:
        tx, ty = zip(*triangle)

        #se é um triangulo correto
        if triangle in triangles:
            x_cumulative += tx
            y_cumulative += ty
            fig.add_trace(go.Scatter(visible=False,
                                    x=x_cumulative + (tx[0],),
                                    y=y_cumulative + (ty[0],),
                                    mode='lines', 
                                    fill='toself',
                                    line=dict(color='green'),
                                    name='Polígono Triangulado',
                                    showlegend=True))
        else:
            fig.add_trace(go.Scatter(visible=False,
                                    x=x + (x[0],) + tx + (tx[0],),
                                    y=y + (y[0],) + ty + (ty[0],),
                                    mode='lines', 
                                    fill='none',
                                    line=dict(color='red',dash='dot'),
                                    name='Vértice dentro do Triangulo',
                                    showlegend=True))
        step_index = step_index + 1

    steps = []
    i = 0
    while i < len(fig.data):
        pular2 = False

        step = dict(
            method="update",
            args=[{"visible": [False] * len(fig.data)},
                {"title": "Passo: " + str(i)}],
        )
        step["args"][0]["visible"][i] = True
        step["args"][0]["visible"][0] = True #Polígono inicial sempre visível

        steps.append(step)
        i += 1

    sliders = [dict(
        active=0,
        currentvalue={"prefix": ""},
        steps=steps
    )]

    fig.update_layout(title='Triangulação por Corte de Orelhas e Coloração dos vértices', 
                      xaxis_title='X', 
                      yaxis_title='Y',
                      sliders=sliders)
    
    fig.show()

#plota somente a coloração dos vértices
def plotVertexColor(polygon, triangles, vertex_colors):
    global triangulos_plot
    fig = go.Figure()
    
    x, y = zip(*polygon)
    
    step_index = 1
    x_cumulative = ()
    y_cumulative = ()

    for triangle in triangulos_plot:
        tx, ty = zip(*triangle)

        if triangle in triangles:
            x_cumulative += tx
            y_cumulative += ty
        step_index = step_index + 1
    
    fig.add_trace(go.Scatter(visible=True,
                                    x=x + (x[0],) + x_cumulative + (tx[0],),
                                    y=y + (y[0],) + y_cumulative + (ty[0],),
                                    mode='lines', 
                                    fill='none',
                                    name='Polígono Triangulado',
                                    showlegend=False))

    for vertex, color in vertex_colors.items():
        fig.add_trace(go.Scatter(visible=False,
                                 x=[vertex[0]],
                                 y=[vertex[1]],
                                 mode='markers', 
                                 marker=dict(color=color, size=20), 
                                 name=color + str(step_index),
                                 showlegend=False))
        step_index = step_index + 1
    
    steps = []
    for i in range(len(fig.data)):
        step = dict(
            method="update",
            args=[{"visible": [False] * len(fig.data)},
                {"title": "Passo: " + str(i)}],
        )
        step["args"][0]["visible"][i] = True

        step["args"][0]["visible"][0] = True 
        for j in range(1,i): #deixa cada vértice colorido visível ao mesmo tempo
            step["args"][0]["visible"][j] = True

        steps.append(step)

    sliders = [dict(
        active=0,
        currentvalue={"prefix": ""},
        steps=steps
    )]

    fig.update_layout(title='Coloração dos vértices', 
                      xaxis_title='X', 
                      yaxis_title='Y',
                      sliders=sliders)
    
    fig.show()

try:
    poligono = readPolygon()
except:
    print("Não foi possível ler o arquivo.")
    exit()

try:
    triangulos = earClippingTriangulation(poligono)
except:
    print("Não foi possível triangular o polígono. Possivel problema\nPolígono não padronizado-Não está ordenado ou não é simples.")
    exit()

try:
    cores_dos_vertices = vertexColorPolygon(poligono, triangulos)
except:
    print("Não foi possível colorir os vértices.")
    exit()


# Use para criar uma plot somente do polígono
# plotPolygon(poligono) 
# Use para criar uma animação somente de triangulação 
# plotTriangulacao(poligono, triangulos) 
# use para criar a animação somente da coloração dos vértices
# plotVertexColor(poligono, triangulos, cores_dos_vertices) 

try:
    plotCompleto(poligono, triangulos, cores_dos_vertices)
except:
    print("Não foi possível plotar o grafico")
    exit()

contagem = {'blue': 0, 'purple': 0, 'yellow': 0}

for cor in cores_dos_vertices.values():
    if cor in contagem:
        contagem[cor] = contagem[cor] + 1


print("São necessários ", min(contagem.values()), " guardas para vigiar a galeria de arte.")