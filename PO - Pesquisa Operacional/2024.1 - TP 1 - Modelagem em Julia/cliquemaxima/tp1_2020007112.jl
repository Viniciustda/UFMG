
# Clique maxima

using JuMP
using HiGHS

struct Grafo
  num_vertices::Int
  arestas::Array{Array{Int}}
end

function le_entrada(arquivo)
  num_vertices = 0
  arestas = [[]]

  for linha in eachline(arquivo)
    aux = split(linha, "\t")

    if aux[1] == "n"
      num_vertices = parse(Int64, aux[2])
      arestas = [[] for _ = 1:num_vertices]
    elseif aux[1] == "e"
      v = parse(Int64, aux[2])
      u = parse(Int64, aux[3])
      push!(arestas[v], u)
      push!(arestas[u], v)
    end
  end

  return Grafo(num_vertices, arestas)
end

arquivo = open(ARGS[1], "r")
dados = le_entrada(arquivo)

modelo = Model(HiGHS.Optimizer)

set_silent(modelo)

@variable(modelo, x[1:dados.num_vertices], Bin)

for i = 1:dados.num_vertices
  for j = 1:dados.num_vertices
    if i != j && !(j in dados.arestas[i])
      @constraint(modelo, x[i] + x[j] <= 1)
    end
  end
end

@objective(modelo, Max, sum(x[i] for i = 1:dados.num_vertices))

optimize!(modelo)

solucao = objective_value(modelo)

println("TP1 2020007112 = ", solucao)