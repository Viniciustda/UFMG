# A-Coloracao

using JuMP
using HiGHS

struct Grafo
  num_vertices::Int
  arestas::Array{Tuple{Int, Int}}
end

function vizinhos(grafo::Grafo, vertice::Int)
  vizinho_vertices = Int[]
  for (u, v) in grafo.arestas
    if u == vertice
      push!(vizinho_vertices, v)
    elseif v == vertice
      push!(vizinho_vertices, u)
    end
  end
  return vizinho_vertices
end

function le_entrada(arquivo)
  num_vertices = 0
  arestas = Tuple{Int, Int}[]
  for linha in eachline(arquivo)
    aux = split(linha, "\t")
    if aux[1] == "n"
      num_vertices = parse(Int64, aux[2])
    elseif aux[1] == "e"
      push!(arestas, (parse(Int, aux[2]), parse(Int, aux[3])))
    end
  end
  return Grafo(num_vertices, arestas)
end

modelo = Model(HiGHS.Optimizer)
set_silent(modelo)

arquivo = open(ARGS[1], "r")

dados = le_entrada(arquivo)

@variable(modelo, x[1:dados.num_vertices], Bin)
@variable(modelo, y[1:dados.num_vertices, 1:dados.num_vertices], Bin)
@variable(modelo, w[1:dados.num_vertices, 1:dados.num_vertices, 1:dados.num_vertices, 1:dados.num_vertices], Bin)

for i in 1:dados.num_vertices
  @constraint(modelo, sum(y[i, j] for j in 1:dados.num_vertices) == 1)
end

@objective(modelo, Max, sum(x[j] for j in 1:dados.num_vertices))

for (i, j) in dados.arestas
  for k in 1:dados.num_vertices
    @constraint(modelo, y[i, k] + y[j, k] <= x[k])
  end
end

for j in 1:dados.num_vertices
  for i in 1:dados.num_vertices
    @constraint(modelo, x[j] >= y[i, j])
  end
end
  
for i in 1:dados.num_vertices
  for j in i+1:dados.num_vertices
    @constraint(modelo, sum(w[u, v, i, j] for u in 1:dados.num_vertices for v in vizinhos(dados, u)) >= x[i] + x[j] - 1)
    for u = 1:dados.num_vertices
      for v in vizinhos(dados, u)
        @constraint(modelo, w[u, v, i, j] <= y[u, i])
        @constraint(modelo, w[u, v, i, j] <= y[v, j])
      end
    end
  end
end

for i = 2:dados.num_vertices
  @constraint(modelo, y[i-1] >= y[i])
end

optimize!(modelo)

solucao = objective_value(modelo)

println("TP1 2020007112 = ", solucao)