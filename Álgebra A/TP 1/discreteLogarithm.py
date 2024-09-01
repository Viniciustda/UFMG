import time
from math import ceil, sqrt
import utils

# Encontra x tal que g^x = h (mod p)
def logaritmoDiscretoForcaBruta(g, h, p,order,tempo_inicial):
  h = h % p
  for x in range(order):
      if pow(g, x, p) == h:
          return x
      if (time.time() - tempo_inicial) >= utils.tempo_limite_logaritmo_discreto:
            return None
  return None # Se nao encontrar


# Função que implementa o algoritmo de Baby-Step Giant-Step (BSGS)
def bsgs(g, h, p,order,tempo_inicial):
    h = h % p
    # Calcula N como o teto da raiz quadrada de (p - 1)
    if order != -1:
        N = ceil(sqrt(order))
    else:
        N = ceil(sqrt(p))

    # Cria uma tabela para armazenar potências de g modulo     p
    tbl = {pow(g, i, p): i for i in range(N)}

    # Calcula c = g^(-N) % p
    c = pow(g, N * (p - 2), p)

    # Loop para calcular (h * c^j) % p e verificar se está na tabela
    for j in range(N):
        y = (h * pow(c, j, p)) % p
        if y in tbl:
            # Se y está na tabela, retorna a solução
            return j * N + tbl[y]
        if (time.time() - tempo_inicial) >= utils.tempo_limite_logaritmo_discreto:
            return None

    # Se nenhuma solução foi encontrada, retorna None
    return None


def pohlig_hellman(g, h, p, factors, initialOrder,tempo_inicial):
    h = h % p
    # Lista para armazenar os resultados parciais
    
    logs = [0] * len(factors)

    # Calcular o logaritmo discreto para cada fator primo
    i = 0
    for q, e in factors:
        for j in range(e):
            gamma = pow(h * pow(g,-logs[i],p), (initialOrder // (q ** (j+1))), p)
            beta = pow(g, initialOrder // q, p)
            if q <= 1000 or q > 1000000000000:
                result = logaritmoDiscretoForcaBruta(beta,gamma,p,q,tempo_inicial)
                if result is None:
                    return None
            else:
                result = bsgs(beta,gamma,p,q,tempo_inicial)
                if result is None:
                    return None
            logs[i] += result * (q ** j)
            if (time.time() - tempo_inicial) >= utils.tempo_limite_logaritmo_discreto:
                return -1
        i += 1

    # Aplicar o Teorema Chinês do Resto para combinar os logaritmos
    x = 0
    for i, (q, e) in enumerate(factors):
        # Calcular o inverso modular
        mi = initialOrder // q**e
        yi = pow(mi, -1, q**e)

        # Combinar os resultados
        x += logs[i] * mi * yi
        x %= p - 1

    return x

def logDiscrete(g,h,p,factors,ordemMinima):
    tempo_inicial = time.time()
    
    if p <= 1000 or (ordemMinima != p -1):
        return logaritmoDiscretoForcaBruta(g,h, p,ordemMinima,tempo_inicial)
    if p <= 1000000000000:
        return bsgs(g, h, p,ordemMinima,tempo_inicial)
    return pohlig_hellman(g, h, p, factors,ordemMinima,tempo_inicial)  
