import time
import sys

class SimplexSolver:
    def __init__(self, c, A, b, decimal=7, detalhamento=False):
        self.decimal = decimal
        self.detalhamento = detalhamento
        self.estados = []
        self.A = A
        self.b = b
        self.c = c
        self.num_restricoes = len(self.A)
        self.num_variaveis = len(self.A[0])
        self.InicializaTableau()

    @property
    def tableau_b(self):
        return [linha[-1] for linha in self.tableau[1:]]

    @property
    def tableau_restricoes(self):
        return [linha[:-1] for linha in self.tableau[1:]]
    
    @property
    def tableau_c(self):
        return self.tableau[0][:-1]

    @property
    def tableau(self):
        return [linha[self.num_restricoes:] for linha in self.tableau_VERO]

    def InicializaTableau(self):
        c_estendido = self.c + [0] * (self.num_restricoes + 1)
        c_estendido = [-x for x in c_estendido]
        
        tableau_fundo = [linha + [1 if i == j else 0 for j in range(self.num_restricoes)] + [self.b[i]] for i, linha in enumerate(self.A)]
        tableau = [c_estendido] + tableau_fundo

        vero = [[0] * self.num_restricoes] + [[1 if i == j else 0 for j in range(self.num_restricoes)] for i in range(self.num_restricoes)]
        self.tableau_VERO = [vero_linha + tableau_linha for vero_linha, tableau_linha in zip(vero, tableau)]
        self.estados.append([linha[:] for linha in self.tableau_VERO])

    def EncontraElementoPivo(self):
        pivo_col = min(range(len(self.tableau_c)), key=lambda i: self.tableau_c[i])

        quocientes = []
        for i in range(self.num_restricoes):
            tmp = self.tableau_restricoes[i][pivo_col]
            if tmp > 0:
                q = self.tableau_b[i] / tmp
                quocientes.append(q)
            else:
                quocientes.append(float('inf'))
            
        pivo_linha = min(range(len(quocientes)), key=lambda i: quocientes[i])

        return pivo_linha + 1, pivo_col + self.num_restricoes
    
    def pivoTableauVERO(self, pivo_linha, pivo_col):
        pivo = self.tableau_VERO[pivo_linha][pivo_col]

        self.tableau_VERO[pivo_linha] = [x / pivo for x in self.tableau_VERO[pivo_linha]]

        for i in range(self.num_restricoes + 1):
            if i != pivo_linha:
                fator = self.tableau_VERO[i][pivo_col]
                self.tableau_VERO[i] = [x - fator * y for x, y in zip(self.tableau_VERO[i], self.tableau_VERO[pivo_linha])]

    def CalculaValorFucaoObjetivo(self):
        vecs, _ = self.EncontraBaseAtual()
        if vecs is None:
            return None
        x = [sum(v[i] * self.b[i] for i in range(len(v))) for v in vecs]
        if len(x) != len(self.c):
            return None
        return sum(self.c[i] * x[i] for i in range(len(x)))
    
    def EncontraBaseAtual(self):
        AI = self.tableau_restricoes
        I = [[1 if i == j else 0 for j in range(self.num_restricoes)] for i in range(self.num_restricoes)]
        indices_bases = []

        for k in range(self.num_restricoes):
            for i in range(len(AI[0])):
                if all(abs(AI[j][i] - I[j][k]) < 1e-7 for j in range(self.num_restricoes)):
                    indices_bases.append(i)
                    break

        if len(indices_bases) != self.num_restricoes:
            return None, None
            
        return [[AI[j][i] for j in range(self.num_restricoes)] for i in indices_bases], indices_bases

    def Viavel(self):
        aux = [linha[:] for linha in self.estados[0]]

        for i in range(self.num_restricoes):
            if aux[i+1][-1] < 0:
                aux[i+1] = [-x for x in aux[i+1][:self.num_restricoes+self.num_variaveis]] + [aux[i+1][-1] * -1]

        aux[0] = [0] * self.num_restricoes + [0] * self.num_variaveis + [1] * self.num_restricoes + [0]

        for i in range(self.num_restricoes):
            aux[0] = [x - y for x, y in zip(aux[0], aux[i+1])]

        while True:
            c = aux[0][self.num_restricoes:-1]
            b = [linha[-1] for linha in aux[1:]]

            if all(x >= 0 for x in c) and all(x >= 0 for x in b):
                break
            
            pivo_col = min(range(len(c)), key=lambda i: c[i]) + self.num_restricoes

            quocientes = []
            for i in range(self.num_restricoes):
                tmp = aux[i+1][pivo_col]
                if tmp > 0:
                    q = b[i] / tmp
                    quocientes.append(q)
                else:
                    quocientes.append(float('inf'))
                
            pivo_linha = min(range(len(quocientes)), key=lambda i: quocientes[i]) + 1
            
            pivo = aux[pivo_linha][pivo_col]

            aux[pivo_linha] = [x / pivo for x in aux[pivo_linha]]

            for i in range(self.num_restricoes + 1):
                if i != pivo_linha:
                    fator = aux[i][pivo_col]
                    aux[i] = [x - fator * y for x, y in zip(aux[i], aux[pivo_linha])]

        if aux[0][-1] < 0:
            print("inviavel")
            print(" ".join(f"{val:.3f}" for val in aux[0][:self.num_restricoes]))
            return False
        return True
    
    def Otimo(self):
        if all(x >= 0 for x in self.tableau_c) and all(x >= 0 for x in self.tableau_b):
            valor_otimo = self.tableau_VERO[0][-1]
            print("otima")
            print(f"{valor_otimo:.3f}")
            
            x = self.GetSolucaoTrivial()

            print(" ".join(f"{val:.3f}" for val in x))
            print(" ".join(f"{val:.3f}" for val in self.tableau_VERO[0][:self.num_restricoes]))
            return True
        return False

    def GetSolucaoTrivial(self):
        x = []
        for i in range(self.num_restricoes, self.num_restricoes + self.num_variaveis):
            if abs(self.tableau_VERO[0][i]) < 1e-7:
                zeros = 0
                indice_um = -1
                for j in range(1, self.num_restricoes + 1):
                    if abs(self.tableau_VERO[j][i]) < 1e-7:
                        zeros += 1
                    elif abs(self.tableau_VERO[j][i] - 1) < 1e-7:
                        indice_um = j
                if zeros == self.num_restricoes - 1 and indice_um != -1:
                    x.append(self.tableau_VERO[indice_um][-1])
                else:
                    x.append(0)
            else:
                x.append(0)
        return x

    def Resolve(self):
        tempo = time.time()

        while True:
            if time.time() - tempo > 0.1:
                if not self.Viavel():
                    return

            if self.Otimo():
                return
            
            for i in range(self.num_restricoes, self.num_restricoes + self.num_variaveis):
                if all(self.tableau_VERO[j][i] <= 0 for j in range(1, self.num_restricoes + 1)):
                    self.Viavel()
                    print("ilimitada")
                    print(" ".join(f"{val:.3f}" for val in self.GetSolucaoTrivial()))
                    print(" ".join(f"{val:.3f}" for val in self.tableau_VERO[0][:self.num_restricoes]))
                    return
           
            self.estados.append([linha[:] for linha in self.tableau_VERO])
            
            pivo_linha, pivo_col = self.EncontraElementoPivo()
            
            self.pivoTableauVERO(pivo_linha, pivo_col)

if __name__ == "__main__":
    linhas = [line.strip() for line in sys.stdin.readlines()]
    
    num_restricoes, num_variaveis = map(float, linhas[0].split())
    
    c = list(map(float, linhas[1].split()))
    
    A_b = [list(map(float, line.split())) for line in linhas[2:]]
    A = [linha[:-1] for linha in A_b]
    b = [linha[-1] for linha in A_b]

    solucao = SimplexSolver(c, A, b, decimal=1, detalhamento=True)

    solucao.Resolve()