# Empacotamento

using JuMP
using HiGHS

mutable struct Packing
  n::Int
  w::Array{Float64}
  B::Int
end

function read_input(file)
  n = 0
  w = []

  for line in eachline(file)
    q = split(line, "\t")

    if q[1] == "n"
      n = parse(Int64, q[2])
      w = [0.0 for i = 1:n]
    elseif q[1] == "o"
      index = parse(Int64, q[2])
      w[index+1] = parse(Float64, q[3])
    end
  end

  return Packing(n, w, 20)
end

function print_solution(data, x, y, sol)
  println("$(sol) CAIXAS")

  for j = 1:data.n
    if value(y[j]) == 1.0
      print("CAIXA $(j) : ")

      for i = 1:data.n
        if value(x[i, j]) == 1.0
          print("$(i - 1) ")
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

for j = 1:data.n
  @constraint(model, sum(x[i, j] * data.w[i] for i = 1:data.n) <= data.B)
end

for i = 1:data.n
  @constraint(model, sum(x[i, j] for j = 1:data.n) == 1)
end

for j = 1:data.n
  for i = 1:data.n
    @constraint(model, y[j] >= x[i, j])
  end
end

@objective(model, Min, sum(y[i] for i = 1:data.n))

# print(model)

optimize!(model)

sol = objective_value(model)
println("TP1 2020007112 = ", sol)

# print_solution(data, x, y, sol)