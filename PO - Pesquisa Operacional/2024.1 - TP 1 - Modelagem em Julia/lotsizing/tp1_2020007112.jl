# LotSizing

using JuMP
using HiGHS

struct LotSizing
  n::Int
  custo_producao::Array{Float64}
  demanda::Array{Float64}
  custo_estocagem::Array{Float64}
  multa::Array{Float64}
end

function le_entrada(arquivo)
  n = 0
  custo_producao = []
  demanda = []
  custo_estocagem = []
  multa = []

  for linha in eachline(arquivo)
    aux = split(linha, "\t")

    if aux[1] == "n"
      n = parse(Int64, aux[2])
      custo_producao = [0.0 for i = 1:n]
      demanda = [0.0 for i = 1:n]
      custo_estocagem = [0.0 for i = 1:n]
      multa = [0.0 for i = 1:n]
    elseif aux[1] == "c"
      index = parse(Int64, aux[2])
      custo_producao[index] = parse(Float64, aux[3])
    elseif aux[1] == "d"
      index = parse(Int64, aux[2])
      demanda[index] = parse(Float64, aux[3])
    elseif aux[1] == "s"
      index = parse(Int64, aux[2])
      custo_estocagem[index] = parse(Float64, aux[3])
    elseif aux[1] == "p"
      index = parse(Int64, aux[2])
      multa[index] = parse(Float64, aux[3])
    end
  end

  return LotSizing(n, custo_producao, demanda, custo_estocagem, multa)
end

arquivo = open(ARGS[1], "r")
dados = le_entrada(arquivo)

modelo = Model(HiGHS.Optimizer)

set_silent(modelo)

@variable(modelo, x[1:dados.n] >= 0) 
@variable(modelo, S[1:dados.n] >= 0)
@variable(modelo, R[1:dados.n] >= 0)

@constraint(modelo, x[1] == dados.demanda[1] + S[1] - R[1])

for i = 2:dados.n
  @constraint(modelo, S[i-1] + x[i] - R[i-1] == dados.demanda[i] + S[i] - R[i])
end

@constraint(modelo, sum(x[i] for i = 1:dados.n) == sum(dados.demanda[i] for i = 1:dados.n))

@objective(modelo, Min, sum((x[i] * dados.custo_producao[i]) + (S[i] * dados.custo_estocagem[i]) + (R[i] * dados.multa[i]) for i = 1:dados.n))

optimize!(modelo)

solucao = objective_value(modelo)

println("TP1 2020007112 = ", solucao)