import numpy as np
import argparse

# Função para ler o arquivo de entrada
def lerArquivo(nome_arquivo):
    """
    Lê e organiza os dados do problema a partir do arquivo de entrada.
    """
    with open(nome_arquivo, 'r') as arquivo:
        linhas = arquivo.readlines()
    
    num_variaveis = int(linhas[0].strip())  # Número de variáveis
    num_restricoes = int(linhas[1].strip())  # Número de restrições
    
    tipos_variaveis = list(map(int, linhas[2].strip().split()))  # Tipos das variáveis
    
    # Tratamento da função objetivo 
    partes_objetivo = linhas[3].strip().split()
    tipo_otimizacao = partes_objetivo[0]
    if tipo_otimizacao in ['max', 'min']:
        funcao_objetivo = np.array(list(map(float, partes_objetivo[1:])))
    else:
        tipo_otimizacao = "max"
        funcao_objetivo = np.array(list(map(float, partes_objetivo[0:])))
    
    # Construindo A, b e sinais
    A = []
    b = []
    sinais = []
    
    for linha in linhas[4:]:
        partes = linha.strip().split()
        coeficientes = list(map(float, partes[:-2]))
        sinal = partes[-2]
        termo_direito = float(partes[-1])
        
        A.append(coeficientes)
        sinais.append(sinal)
        b.append(termo_direito)
    
    A = np.array(A)
    b = np.array(b)

    variavel = 'x'
    valor_objetivo = 0
    
    return {
        'num_variaveis': num_variaveis,
        'num_restricoes': num_restricoes,
        'tipo_otimizacao': tipo_otimizacao,
        'funcao_objetivo': funcao_objetivo,
        'A': A,
        'sinais': sinais,
        'b': b,
        'variavel': variavel,
        'tipos_variaveis': tipos_variaveis,
        'valor_objetivo': valor_objetivo
    }
# fim lerArquivo

# Função para imprimir a PL
def imprimePL(pl):
    """
    Formata e imprime a PL.
    """
    contador = 0
    print(pl['tipo_otimizacao'], pl['funcao_objetivo'], pl['variavel'])
    print("sujeito a ")
    for i in range(pl['num_restricoes']):
        print(pl['A'][i,:], "x", pl['sinais'][i], pl['b'][i])
    for i in range(pl['num_variaveis']):
        if pl['tipos_variaveis'][i] == 1:
            print(f"{pl['variavel']}{contador} >= 0")
        elif pl['tipos_variaveis'][i] == 0:
            print(f"{pl['variavel']}{contador} livre")
        else:
            print(f"{pl['variavel']}{contador} < 0")
        contador += 1
    print("\nNumero de variaveis:", pl['num_variaveis'])
    print("\n---------------------------------")
# fim imprimePL

# Função para transformar em FPI
def fpi(pl):
    """
    Recebe uma PL e coloca ela em FPI.
    """
    num_variaveis = pl['num_variaveis']
    num_restricoes = pl['num_restricoes']
    tipo_otimizacao = pl['tipo_otimizacao']
    funcao_objetivo = pl['funcao_objetivo'].copy()
    A = pl['A']
    sinais = pl['sinais']
    b = pl['b']
    variavel = pl['variavel']
    tipos_variaveis = pl['tipos_variaveis']
    valor_objetivo = pl['valor_objetivo']

    contador = 0

    # Se for preciso, transforma em problema de maximizacao
    if tipo_otimizacao == "min":
        tipo_otimizacao = "max"
        funcao_objetivo = -funcao_objetivo
    
    # Troca sinais para igualdade e adiciona variaveis de folga
    for i, sinal in enumerate(sinais):
        if sinal == '<=' :
            sinais[i] = '='
            coluna_folga = np.zeros((num_restricoes, 1))  
            coluna_folga[i] = 1 
            A = np.hstack([A, coluna_folga])
            tipos_variaveis.append(1)
            num_variaveis += 1
            contador += 1
        elif sinal == '>=':
            sinais[i] = '='
            coluna_folga = np.zeros((num_restricoes, 1))  
            coluna_folga[i] = -1 
            A = np.hstack([A, coluna_folga])
            tipos_variaveis.append(1)
            num_variaveis += 1
            contador += 1

    funcao_objetivo = np.hstack([funcao_objetivo, np.zeros(contador)])

    # Transforma variaveis livres e negativas
    for i, tipo in enumerate(tipos_variaveis):
        if tipo == -1:
            tipos_variaveis[i] = 1
            A[:,i] *= -1
        elif tipo == 0:
            tipos_variaveis[i] = 1
            tipos_variaveis = np.insert(tipos_variaveis, i+1, 1)
            num_variaveis += 1
            A = np.insert(A, i+1, -A[:, i], axis = 1)
            funcao_objetivo = np.insert(funcao_objetivo, i+1, -funcao_objetivo[i])

    return {
        'num_variaveis': num_variaveis,
        'num_restricoes': num_restricoes,
        'tipo_otimizacao': tipo_otimizacao,
        'funcao_objetivo': funcao_objetivo,
        'A': A,
        'sinais': sinais,
        'b': b,
        'variavel': variavel,
        'tipos_variaveis': tipos_variaveis,
        'valor_objetivo': valor_objetivo
    }
# fim fpi

# Função para verificar se PL possui base obvia
def verificaBaseObvia(pl):
    """
    Verifica se a PL possui uma base óbvia e retorna os índices das colunas que formam a base.
    Uma base óbvia ocorre quando há uma matriz identidade completa, mesmo que as colunas não sejam consecutivas.
    """
    funcao_objetivo = pl['funcao_objetivo']
    A = pl['A']
    num_restricoes = pl['num_restricoes']
    identidade = np.eye(num_restricoes)
    
    id_colunas_base = []
    colunas_selecionadas = []
    for i in range(A.shape[1]):
        if funcao_objetivo[i] == 0:
            if any(np.array_equal(A[:, i], identidade[:, j]) for j in range(num_restricoes)):
                colunas_selecionadas.append(A[:, i])
                id_colunas_base.append(i)
    
    if len(colunas_selecionadas) >= num_restricoes:
        submatriz = np.array(colunas_selecionadas).T
        _, indices_unicos = np.unique(submatriz, axis=1, return_index=True)
        submatriz = submatriz[:, sorted(indices_unicos)]
        id_colunas_base = [id_colunas_base[i] for i in sorted(indices_unicos)]

        ordem = [np.argmax(col) for col in submatriz.T] 
        ordem = np.argsort(ordem)
        submatriz = submatriz[:, ordem]

        if np.array_equal(submatriz, identidade):
            return True, id_colunas_base
    
    return False, id_colunas_base
# fim verificaBaseObvia

# Função para imprimir o tableau
def imprimeTableau(pl, decimais=3, digitos=7):
    """
    Imprime a PL no tableau.
    """
    formato = f"{{:>{digitos}.{decimais}f}}"
    linha_superior = f"\n| {formato.format(1)} | {' '.join([formato.format(num) for num in pl['funcao_objetivo']])} | {formato.format(pl['valor_objetivo'])} |"
    print(linha_superior)
    largura_total = len(linha_superior)
    print("|" + "-" * (largura_total - 3) + "|")
    for i in range(pl['num_restricoes']):
        linha = f"| {formato.format(0)} | {' '.join([formato.format(num) for num in pl['A'][i, :]])} | {formato.format(pl['b'][i])} |"
        print(linha)
# fim imprimeTableau

# Função para criar PL auxiliar
def plAuxiliar(pl):
    """
    Cria uma PL auxiliar a partir da PL original.
    """
    num_variaveis = pl['num_variaveis']
    num_restricoes = pl['num_restricoes']
    tipo_otimizacao = pl['tipo_otimizacao']
    funcao_objetivo = pl['funcao_objetivo'].copy()
    A = pl['A']
    sinais = pl['sinais']
    b = pl['b']
    variavel = pl['variavel']
    tipos_variaveis = pl['tipos_variaveis']
    valor_objetivo = pl['valor_objetivo']

    base = [num_variaveis + i for i in range(num_restricoes)]

    funcao_objetivo[:] = 0

    num_variaveis += num_restricoes
    for i in range(num_restricoes):
        funcao_objetivo = np.append(funcao_objetivo, -1)
        tipos_variaveis.append(1)
    
    identidade = np.eye(num_restricoes)
    A = np.hstack((A, identidade))

    pl_aux = {
        'num_variaveis': num_variaveis,
        'num_restricoes': num_restricoes,
        'tipo_otimizacao': tipo_otimizacao,
        'funcao_objetivo': funcao_objetivo,
        'A': A,
        'sinais': sinais,
        'b': b,
        'variavel': variavel,
        'tipos_variaveis': tipos_variaveis,
        'valor_objetivo': valor_objetivo,
        'base': base
    }
    return pl_aux
# fim plAuxiliar

# Função simplex
def simplex(pl, decimais=3, digitos=7, policy="largest"):
    """
    Implementa o método Simplex.
    
    
    1- funcao objetivo multiplicada por -1
    2- colocar na forma canonica
    Pivoteamente - Enquanto houver elementos negativos na funcao objetivo:
        3- escolhemos coluna (largest - com maior valor negativo em modulo; smallest - com menor valor negativo em modulo; bland - menor indice)
        4- escolhemos linha da 2ª a ultima - a razão entre os elementos da última coluna e da coluna escolhida seja o menor possível mas que nao seja < 0
        5- eliminacao gaussiana - ate que pivo seja 1 e o resto 0 - qual elemento esta sendo pivoteado e imprimir tableau a cada linha alterada
    """    
    num_variaveis = pl['num_variaveis']
    num_restricoes = pl['num_restricoes']
    A = pl['A']
    b = pl['b']
    variavel = pl['variavel']
    base = pl['base']
    valor_objetivo = 0.0
    # Multiplica funacaoobjetivo por -1
    funcao_objetivo = -pl['funcao_objetivo']

    print("\nSimplex:")

    # Inicializar variáveis básicas e não básicas
    variaveis_basicas = [f"{variavel}{i}" for i in base]
    variaveis_nao_basicas = []
    for i in range(num_variaveis):
        if i in base:
            variaveis_nao_basicas.append("")
        else:
            variaveis_nao_basicas.append(f"{variavel}{i}")

    print("\nTableau inicial:")
    print(f"\nVariaveis basicas: {variaveis_basicas}")
    print("Variaveis nao basicas:", [variaveis_nao_basicas[i] for i in range(num_variaveis) if variaveis_nao_basicas[i] != ""])
    pl['funcao_objetivo'] = funcao_objetivo
    imprimeTableau(pl, decimais, digitos)

    # Coloca na forma canonica
    if all(funcao_objetivo >= 0):
        print("\nColoca em forma canonica:")

        for coluna_base in base:
            fator = funcao_objetivo[coluna_base]
            if not np.isclose(fator, 0.0): 
                
                linha_identidade = np.where(A[:, coluna_base] == 1.0)[0]
                if linha_identidade.size > 0:  # Se encontrou uma linha válida
                    linha_identidade = linha_identidade[0]  # Pegue o índice da linha
                    funcao_objetivo -= fator * A[linha_identidade, :]

        pl['funcao_objetivo'] = funcao_objetivo
        imprimeTableau(pl, decimais, digitos)
    else:
        print("\nJa esta na forma canonica.")

    # Metodo dual - Pivoteamento
    if any(b < 0):
        print("\nMetodo dual:")
    while any(b < 0):

        if policy == 'largest':
            linha_pivo = np.argmin(b * (b < 0))
        elif policy == 'smallest':
            linha_pivo = np.argmax(b * (b < 0))
        else:  # Bland
            for i in range(len(b)):
                if b[i] < 0:
                    linha_pivo = i
                    break

        if not any(A[linha_pivo, :] < 0):
            status = "inviavel"
            return status, None, None, None
        
        # Escolha da variável que sai da base
        for i in range(num_variaveis):
            if A[linha_pivo, i] < 0:
               coluna_pivo = i
               break
        
        # Variável que entra
        variavel_entra = variaveis_nao_basicas[coluna_pivo]
        
        # Variável que sai
        variavel_sai = variaveis_basicas[linha_pivo]
        
        # Realiza o pivoteamento
        pivo = A[linha_pivo, coluna_pivo]
        A[linha_pivo, :] /= pivo
        b[linha_pivo] /= pivo
        
        for i in range(num_restricoes):
            if i != linha_pivo:
                fator = A[i, coluna_pivo]
                A[i, :] -= fator * A[linha_pivo, :]
                b[i] -= fator * b[linha_pivo]
        
        fator_objetivo = funcao_objetivo[coluna_pivo]
        funcao_objetivo -= fator_objetivo * A[linha_pivo, :]
        valor_objetivo -= fator_objetivo * b[linha_pivo]

        # Atualizar listas de variáveis
        variavel_temp = variaveis_basicas[linha_pivo]
        variaveis_basicas[linha_pivo] = variaveis_nao_basicas[coluna_pivo]
        variaveis_nao_basicas[coluna_pivo] = ""

        for i in range(num_variaveis):
            if f"{variavel}{i}" == variavel_temp:
                variaveis_nao_basicas[i] = variavel_temp
        
        # Imprime o tableau após cada iteração
        formato = f"{{:>{digitos}.{decimais}f}}"
        print("\nPivoteamento:")
        print(f"\nVariaveis basicas: {variaveis_basicas}")
        print("Variaveis nao basicas:", [variaveis_nao_basicas[i] for i in range(num_variaveis) if variaveis_nao_basicas[i] != ""])
        print(f"\nVariavel que saiu: {variavel_sai}") 
        print(f"Variavel que entrou: {variavel_entra}")
        print(f"Pivo: {formato.format(pivo)}")
        pl['A'] = A
        pl['b'] = b
        pl['funcao_objetivo'] = funcao_objetivo
        pl['valor_objetivo'] = valor_objetivo
        imprimeTableau(pl, decimais, digitos)

    # Metodo primal - Pivoteamento
    print("\nMetodo primal:")
    while any(funcao_objetivo < 0):

        if policy == 'largest':
            coluna_pivo = np.argmin(funcao_objetivo * (funcao_objetivo < 0))
        elif policy == 'smallest':
            coluna_pivo = np.argmax(funcao_objetivo * (funcao_objetivo < 0))
        else:  # Bland
            for i in range(len(funcao_objetivo)):
                if funcao_objetivo[i] < 0:
                    coluna_pivo = i
                    break
        
        # Escolha da variável que sai da base
        razoes = []
        for i in range(num_restricoes):
            if A[i, coluna_pivo] > 0:
                razoes.append(b[i] / A[i, coluna_pivo])
            else:
                razoes.append(np.inf)
        
        linha_pivo = np.argmin(razoes)
        if razoes[linha_pivo] == np.inf:
            status = "ilimitado"
            return status, None, None, None
        
        # Variável que entra
        variavel_entra = variaveis_nao_basicas[coluna_pivo]
        
        # Variável que sai
        variavel_sai = variaveis_basicas[linha_pivo]
        
        # Realiza o pivoteamento
        pivo = A[linha_pivo, coluna_pivo]
        if np.isclose(pivo, 0.0):
            status = "ilimitado"
            return status, None, None, None
        
        A[linha_pivo, :] /= pivo
        b[linha_pivo] /= pivo
        
        for i in range(num_restricoes):
            if i != linha_pivo:
                fator = A[i, coluna_pivo]
                A[i, :] -= fator * A[linha_pivo, :]
                b[i] -= fator * b[linha_pivo]
        
        fator_objetivo = funcao_objetivo[coluna_pivo]
        funcao_objetivo -= fator_objetivo * A[linha_pivo, :]
        valor_objetivo -= fator_objetivo * b[linha_pivo]

        # Atualizar listas de variáveis
        variavel_temp = variaveis_basicas[linha_pivo]
        variaveis_basicas[linha_pivo] = variaveis_nao_basicas[coluna_pivo]
        variaveis_nao_basicas[coluna_pivo] = ""

        for i in range(num_variaveis):
            if f"{variavel}{i}" == variavel_temp:
                variaveis_nao_basicas[i] = variavel_temp
        
        # Imprime o tableau após cada iteração
        formato = f"{{:>{digitos}.{decimais}f}}"
        print("\nPivoteamento:")
        print(f"\nVariaveis basicas: {variaveis_basicas}")
        print("Variaveis nao basicas:", [variaveis_nao_basicas[i] for i in range(num_variaveis) if variaveis_nao_basicas[i] != ""])
        print(f"\nVariavel que saiu: {variavel_sai}") 
        print(f"Variavel que entrou: {variavel_entra}")
        print(f"Pivo: {formato.format(pivo)}")
        pl['A'] = A
        pl['b'] = b
        pl['funcao_objetivo'] = funcao_objetivo
        pl['valor_objetivo'] = valor_objetivo
        imprimeTableau(pl, decimais, digitos)
    
    solucao = np.zeros(num_variaveis)
    for i in range(num_restricoes):
        for j in range(num_variaveis):
            if np.isclose(A[i, j], 1) and np.allclose(A[:, j], np.eye(num_restricoes)[:, i]):
                solucao[j] = b[i]

    if valor_objetivo < 0:
        status = "inviavel"
    else:
        status = "otimo"

    # Verifica se tem multiplas solucoes
    multiplas_solucoes = False
    for i in range(num_variaveis):
        if variaveis_nao_basicas[i] and np.isclose(funcao_objetivo[i], 0):
            multiplas_solucoes = True
            break
    
    segunda_solucao = np.zeros(num_variaveis)

    if multiplas_solucoes:
               
        for coluna_pivo in range(num_variaveis):
            if variaveis_nao_basicas[coluna_pivo] == f"{variavel}{coluna_pivo}" and np.isclose(funcao_objetivo[coluna_pivo], 0):
                break
        
        # Escolha da variável que sai da base
        razoes = []
        for i in range(num_restricoes):
            if A[i, coluna_pivo] > 0:
                razoes.append(b[i] / A[i, coluna_pivo])
            else:
                razoes.append(np.inf)
        
        linha_pivo = np.argmin(razoes)
        if razoes[linha_pivo] != np.inf:
        
            # Variável que entra
            variavel_entra = variaveis_nao_basicas[coluna_pivo]
            
            # Variável que sai
            variavel_sai = variaveis_basicas[linha_pivo]
            
            # Realiza o pivoteamento
            pivo = A[linha_pivo, coluna_pivo]
            if not np.isclose(pivo, 0.0):
            
                A[linha_pivo, :] /= pivo
                b[linha_pivo] /= pivo
                
                for i in range(num_restricoes):
                    if i != linha_pivo:
                        fator = A[i, coluna_pivo]
                        A[i, :] -= fator * A[linha_pivo, :]
                        b[i] -= fator * b[linha_pivo]
                
                fator_objetivo = funcao_objetivo[coluna_pivo]
                funcao_objetivo -= fator_objetivo * A[linha_pivo, :]
                valor_objetivo -= fator_objetivo * b[linha_pivo]

                # Atualizar listas de variáveis
                variavel_temp = variaveis_basicas[linha_pivo]
                variaveis_basicas[linha_pivo] = variaveis_nao_basicas[coluna_pivo]
                variaveis_nao_basicas[coluna_pivo] = ""

                for i in range(num_variaveis):
                    if f"{variavel}{i}" == variavel_temp:
                        variaveis_nao_basicas[i] = variavel_temp
                
                # Imprime o tableau após cada iteração
                formato = f"{{:>{digitos}.{decimais}f}}"
                print("\nPivoteamento:")
                print(f"\nVariaveis basicas: {variaveis_basicas}")
                print("Variaveis nao basicas:", [variaveis_nao_basicas[i] for i in range(num_variaveis) if variaveis_nao_basicas[i] != ""])
                print(f"\nVariavel que saiu: {variavel_sai}") 
                print(f"Variavel que entrou: {variavel_entra}")
                print(f"Pivo: {formato.format(pivo)}")
                pl['A'] = A
                pl['b'] = b
                pl['funcao_objetivo'] = funcao_objetivo
                pl['valor_objetivo'] = valor_objetivo
                imprimeTableau(pl, decimais, digitos)

                status = "otimo (multiplos)"
                for i in range(num_restricoes):
                    for j in range(num_variaveis):
                        if np.isclose(A[i, j], 1) and np.allclose(A[:, j], np.eye(num_restricoes)[:, i]):
                            segunda_solucao[j] = b[i]

    return status, pl, solucao, segunda_solucao
# fim simplex

# Função principal
def main():
    """
    Função principal para gerenciar as etapas do Simplex.
    """
    parser = argparse.ArgumentParser(description='Implementação do método Simplex.')
    parser.add_argument('filename', type=str, help='Nome do arquivo LP de entrada')
    parser.add_argument('--decimals', type=int, default=3, help='Número de casas decimais para impressão')
    parser.add_argument('--digits', type=int, default=7, help='Número total de dígitos para impressão')
    parser.add_argument('--policy', type=str, choices=['largest', 'bland', 'smallest'], default='largest', help='Política de escolha de pivô')
    
    args = parser.parse_args()
    
    # Lê entrada
    pl_original = lerArquivo(args.filename)
    print("\nPL original:\n")
    imprimePL(pl_original)
    
    # Transforma em FPI
    pl_fpi = fpi(pl_original)
    print("\nPL em FPI:\n")
    imprimePL(pl_fpi)

    possui_base_obvia, base = verificaBaseObvia(pl_fpi)    

    # Resolve a PL auxiliar se for preciso
    if (possui_base_obvia):
        print(f"\nPL possui base obvia:\n")
        print(base)
        print("\n---------------------------------")

        pl = pl_fpi
        pl['base'] = base
    else:
        print(f"\nPL nao possui base obvia, entao sera usada a PL auxiliar.")
        print("\n---------------------------------")
                
        pl_aux = plAuxiliar(pl_fpi)

        print("\nPL auxiliar:\n")
        imprimePL(pl_aux)

        print("\nPL auxiliar - Aplicacao do Simplex:")
        _, pl_aux_final, solucao_pl_aux, _ = simplex(pl_aux, args.decimals, args.digits, args.policy)

        if pl_aux_final['valor_objetivo'] == 0:
            print("\nPL auxiliar - Solucao basica inicial encontrada:\n")
            print(f"{pl_aux_final['variavel']} = {solucao_pl_aux}")
            print("\n---------------------------------")

            _, base = verificaBaseObvia(pl_aux_final)

            pl = pl_fpi
            pl['A'] = pl_aux_final['A'][:, :-pl['num_restricoes']]
            pl['b'] = pl_aux_final['b']
            pl['base'] = base

            print("\nPL com solucao basica inicial:\n")
            imprimePL(pl)

        elif pl_aux_final['valor_objetivo'] < 0:
            print("\nStatus: inviavel")
            return
        else:
            print(pl_aux_final['valor_objetivo'])
            raise ValueError("Erro: Valor objetivo da PL auxiliar é inválido.")

    # Aplica o simplex    
    status, pl_final, solucao, segunda_solucao = simplex(pl, args.decimals, args.digits, args.policy)

    if status == "ilimitado" or status == "inviavel":
        print(f"\nStatus: {status}")
        return

    # Descobre solucao
    n = pl_final['num_variaveis'] - pl_original['num_variaveis']
    solucao = solucao[:pl_original['num_variaveis']]
    solucao_dual = pl_final['funcao_objetivo'][-n:]

    segunda_solucao = segunda_solucao[:pl_original['num_variaveis']]
    segunda_solucao_dual = pl_final['funcao_objetivo'][-n:]
    
    # Imprime resultado
    np.set_printoptions(formatter={'float': lambda x: f"{x:{args.digits}.{args.decimals}f}"})
    formato = f"{{:>{args.digits}.{args.decimals}f}}"

    print(f"\nStatus: {status}")
    if status == "otimo":
        print(f"Objetivo: {formato.format(pl_final['valor_objetivo'])}")
        print("Solucao:")
        print(solucao)
        print("Dual:")
        print(solucao_dual)

    elif status == "otimo (multiplos)": 
        print(f"Objetivo: {pl_final['valor_objetivo']}")
        print("Solucoes:")
        print(solucao)
        print(segunda_solucao)
        print("Dual:")
        print(solucao_dual)
        print(segunda_solucao_dual)
        
# fim main

if __name__ == "__main__":
    main()