# TP 1 - Modelagem em Julia

**TP 1: Integer programming models in Julia**  
Course: [PO - Pesquisa Operacional](../README.md) (Operations Research), 2024/1

Integer programming models for five problems, solved with JuMP and the HiGHS solver: bin packing (`empacotamento`), maximum clique (`cliquemaxima`), lot sizing (`lotsizing`), graph coloring (`coloracao`) and a coloring variant (`A-coloracao`).

## Files

- [`A-coloracao/`](A-coloracao)
- [`cliquemaxima/`](cliquemaxima)
- [`coloracao/`](coloracao)
- [`empacotamento/`](empacotamento)
- [`lotsizing/`](lotsizing)

## How to run

```bash
julia -e 'using Pkg; Pkg.add(["JuMP", "HiGHS"])'
cd empacotamento
julia tp1_2020007112.jl INST1_PB1.txt
```

## Notes

- In each folder, `tp1_2020007112.jl` is the submitted model, `INST*` are the test instances and `SOL*` the expected answers.

Comments in the code and reports are in Portuguese unless noted otherwise.
