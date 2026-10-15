# Empacotamento

using JuMP
using HiGHS

struct Pacote
  num_objetos::Int
  pesos::Array{Float64}
  capacidade::Int
end

function le_entrada(arquivo)
  num_objetos = 0
  pesos = []

  for linha in eachline(arquivo)
    aux = split(linha, "\t")

    if aux[1] == "n"
      num_objetos = parse(Int64, aux[2])
      pesos = [0.0 for i = 1:num_objetos]
    elseif aux[1] == "o"
      index = parse(Int64, aux[2])
      pesos[index+1] = parse(Float64, aux[3])
    end
  end

  return Pacote(num_objetos, pesos, 20)
end

arquivo = open(ARGS[1], "r")
dados = le_entrada(arquivo)

modelo = Model(HiGHS.Optimizer)

set_silent(modelo)

@variable(modelo, x[1:dados.num_objetos, 1:dados.num_objetos], Bin)
@variable(modelo, y[1:dados.num_objetos], Bin)

for j = 1:dados.num_objetos
  @constraint(modelo, sum(x[i, j] * dados.pesos[i] for i = 1:dados.num_objetos) <= dados.capacidade)
end

for i = 1:dados.num_objetos
  @constraint(modelo, sum(x[i, j] for j = 1:dados.num_objetos) == 1)
end

for j = 1:dados.num_objetos
  for i = 1:dados.num_objetos
    @constraint(modelo, y[j] >= x[i, j])
  end
end

@objective(modelo, Min, sum(y[i] for i = 1:dados.num_objetos))

optimize!(modelo)

solucao = objective_value(modelo)

println("TP1 2020007112 = ", solucao)