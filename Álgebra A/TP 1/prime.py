import utils

def findNextPrime(n):
    if n <=0: return (1,0)
    if n == 1: return (2,0)
    
    if n % 2 == 0:
        n = n+1
    else:
        n = n+2
    
    numeroTestes = 0
    while(True):
        isPrime = True
        for prime in utils.primes:
            if n == prime: return n,numeroTestes
            if n % prime == 0:
                isPrime = False
                break
        if(isPrime):
            isPrime,testes = millerRabin(n)
            numeroTestes += testes
            if(isPrime): return n,numeroTestes
        n = n+2

def millerRabin(n):
    k = 0
    m = n - 1
    numeroTestes = 0
    while ((m%2) == 0):
        m //= 2
        k += 1
    for prime in utils.primes:
        numeroTestes+=1
        x = utils.power(prime,m,n)
        if (x == 1) or (x == (n-1)):
            return True,numeroTestes
        for j in range(1,k-1):
            x = (x * x)% n
            if x == (n-1):return True,numeroTestes
            if x == 1: break
    return False,numeroTestes

def printPrime(primo,qtUsosMiller):
    print('Proximo primo:',primo)
    print('Numero de usos do Miller Rabin:',qtUsosMiller)