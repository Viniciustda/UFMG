import utils

def findGenerator(v, factors, restoFatoracao):
    geradorEstimado = 1
    combinado = 1
    potenciasCombinadas = 1
    for factor in factors:
        division = (v-1)//factor[0]
        divisionPower = (v-1)//(factor[0] ** factor[1])
        combinado *= factor[0]
        potenciasCombinadas *= (factor[0] ** factor[1])
        for i in range(2,v-1):
            if utils.power(i,division,v) != 1:
                geradorEstimado *= (utils.power(i, divisionPower, v))
                geradorEstimado = geradorEstimado % v
                break
    if restoFatoracao == 1: return geradorEstimado,(v-1)
    for i in range(2,v-1):
        if utils.power(i,combinado,v) != 1:
            geradorEstimado *= (utils.power(i, potenciasCombinadas, v))
            geradorEstimado = geradorEstimado % v
            break
    return geradorEstimado,potenciasCombinadas

def printGenerator(gerador,ordemMinima,restoFatoracao):
    if(restoFatoracao != 1):
        print('Não foi possível computar se g é gerador ou não')
        print('Elemento com ordem alta:',gerador)
        print('Ordem mínima:',ordemMinima)
    else:
        print('Gerador:',gerador)
