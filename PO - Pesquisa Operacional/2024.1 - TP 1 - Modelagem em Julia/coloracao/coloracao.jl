# Coloracao

using JuMP
using HiGHS

mutable struct Graph
  n::Int
  g::Array{Array{Int}}
end

function read_input(file)
  n = 0
  g = [[]]

  for line in eachline(file)
    q = split(line, "\t")

    if q[1] == "n"
      n = parse(Int64, q[2])
      g = [[] for _ = 1:n]
    elseif q[1] == "e"
      v = parse(Int64, q[2])
      u = parse(Int64, q[3])
      push!(g[v], u)
      push!(g[u], v)
    end
  end

  return Graph(n, g)
end

function print_solution(data, x, y, sol)
  println("$(sol) CORES")

  for i = 1:data.n
    if value(y[i]) > 0.5
      print("COR $(i) : ")
      for j = 1:data.n
        if value(x[j, i]) > 0.5
          print("$(j) ")
        end
      end
      println()
    end
  end
end

file = open(ARGS[1], "r")
data = read_input(file)

model = Model(HiGHS.Optimizer)

# remover essa linha volta com os logs do HiGHS
set_silent(model)

@variable(model, x[1:data.n, 1:data.n], Bin)
@variable(model, y[1:data.n], Bin)

for i = 1:data.n
  @constraint(model, sum(x[i, j] for j = 1:data.n) == 1)
end

for i = 1:data.n
  for j in data.g[i]
    for k = 1:data.n
      @constraint(model, x[i, k] + x[j, k] <= 1)
    end
  end
end

for j = 1:data.n
  for i = 1:data.n
    @constraint(model, y[j] >= x[i, j])
  end
end

@objective(model, Min, sum(y[j] for j = 1:data.n))

# print(model)

optimize!(model)

sol = objective_value(model)
println("TP1 2020007112 = ", sol)

# print_solution(data, x, y, sol)