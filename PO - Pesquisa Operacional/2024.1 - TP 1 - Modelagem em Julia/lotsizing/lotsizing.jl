# LotSizing

using JuMP
using HiGHS

mutable struct LotSizing
  n::Int
  c::Array{Float64} # custo de producao
  d::Array{Float64} # demanda
  h::Array{Float64} # custo de estocagem
  p::Array{Float64} # valor da multa
end

function read_input(file)
  n = 0
  c = []
  d = []
  h = []
  p = []

  for line in eachline(file)
    q = split(line, "\t")

    if q[1] == "n"
      n = parse(Int64, q[2])
      c = [0.0 for i = 1:n]
      d = [0.0 for i = 1:n]
      h = [0.0 for i = 1:n]
      p = [0.0 for i = 1:n]
    elseif q[1] == "c"
      index = parse(Int64, q[2])
      c[index] = parse(Float64, q[3])
    elseif q[1] == "d"
      index = parse(Int64, q[2])
      d[index] = parse(Float64, q[3])
    elseif q[1] == "s"
      index = parse(Int64, q[2])
      h[index] = parse(Float64, q[3])
    elseif q[1] == "p"
      index = parse(Int64, q[2])
      p[index] = parse(Float64, q[3])
    end
  end

  return LotSizing(n, c, d, h, p)
end

function print_solution(data, x, sol)
  println("SOLUÇÃO: $(sol)")

  for i = 1:data.n
    println("PRODUÇÃO PERIODO $(i) : $(value(x[i]))")
  end
end

file = open(ARGS[1], "r")
data = read_input(file)

model = Model(HiGHS.Optimizer)

# remover essa linha volta com os logs do HiGHS
set_silent(model)

@variable(model, x[1:data.n] >= 0)  # quantidade produzida
@variable(model, S[1:data.n] >= 0)  # quantidade armazenada
@variable(model, R[1:data.n] >= 0)  # quantidade nao entregue

@constraint(model, x[1] == data.d[1] + S[1] - R[1])

for i = 2:data.n
  @constraint(model, S[i-1] + x[i] - R[i-1] == data.d[i] + S[i] - R[i])
end

@constraint(model, sum(x[i] for i = 1:data.n) == sum(data.d[i] for i = 1:data.n))

@objective(
  model,
  Min,
  sum((x[i] * data.c[i]) + (S[i] * data.h[i]) + (R[i] * data.p[i]) for i = 1:data.n)
)

# print(model)

optimize!(model)

sol = objective_value(model)
println("TP1 2020007112 = ", sol)

# print_solution(data, x, sol)