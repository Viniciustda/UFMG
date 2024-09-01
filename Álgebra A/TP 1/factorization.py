import prime
import time
import utils

def factorazation(v):
    tempo_inicial = time.time()
    factors = []
    for factor in utils.primes:
        if v % factor == 0:
            v //= factor
            quantidade = 1
            while v % factor == 0:
                v //= factor
                quantidade +=1
            factors.append((factor,quantidade))
            if v == 1: return factors,v
    
    increments = [4,2,4,6,2,6,4,2]
    factor = utils.primes[len(utils.primes) - 1] + 2
    i = 0
    while factor < 1000000:
        if v % factor == 0:
            v //= factor
            quantidade = 1
            while v % factor == 0:
                v //= factor
                quantidade +=1
            factors.append((factor,quantidade))
            if v==1 :return factors,v
        if i == 8:i = 0
        factor += increments[i]
        i += 1
        
    while v != 1:
        isPrime,testes = prime.millerRabin(v)
        if(isPrime):
            factors.append((v,1))
            v = 1
            return factors,v
        for values in utils.primes:
            result = polardRho(tempo_inicial,v,values,c=1)
            if result == 0: return factors,v 
            if result != -1:
                v //= result 
                quantidade = 1
                while v % result  == 0:
                    v //= result 
                    quantidade +=1
                factors.append((result ,quantidade))
                break
    return factors,v 

def polardRho(tempo_inicial,v,x,c=1):
    d = 1
    y = x
    while d == 1:
        x = (utils.power(x,2,v) + c + v) % v
        y = (utils.power(y,2,v) + c + v) % v
        y = (utils.power(y,2,v) + c + v) % v
        d = utils.gcd(abs(x - y),v)
        if d == v: return -1
        if (time.time() - tempo_inicial) >= utils.tempo_limite_fatoracao:
            return 0
    return d