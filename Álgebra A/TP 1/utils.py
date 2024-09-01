# -- Primeiros 20 primos, razoavel para o teste de Miller Rabin 
primes = [2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71]

#Tempo de parada para a fatoração, atualmente determinada como 5 minutos
tempo_limite_fatoracao = 300

#Tempo de parada para a fatoração, atualmente determinada como 5 minutos
tempo_limite_logaritmo_discreto = 600

#Calcular a^k mod n
def power(a,k,n):
    res = 1     
    a = a % n  
    while k > 0:
        if (k & 1):res = (res*a) % n
        k = k >> 1
        a = (a*a) % n
    return res

def gcd(a,b):
    if a == 0:
        return b
    return gcd(b % a, a)