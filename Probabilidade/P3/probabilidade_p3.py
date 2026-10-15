import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import binom, expon, uniform, norm

# Função para gerar amostras e plotar histogramas
def AmostrasHistogramas(distribution, params, n, m, titulo):

  # (a) Gerar amostras
  amostras = distribution.rvs(size=(m, n), *params)

  # (b) Plotar histogramas das amostras
  plt.figure()
  for i in range(m):
        plt.hist(amostras[i, :], bins=30, density=True, alpha=0.5, label=f'Amostra {i + 1}')

  # (c) Calcular a média amostral de cada uma das m amostras de tamanho n geradas
  medias_amostrais = np.mean(amostras, axis=1)

  # (d) Adicionar médias amostrais ao gráfico
  for i, media in enumerate(medias_amostrais):
    plt.axvline(x=media, color=f'C{i}', linestyle='--', linewidth=2, label=f'Média Amostral {i + 1}')

  # Plotar distribuição normal teórica
  media_real, std_real = distribution.stats(*params, moments='mv')
  norm_dist = norm(loc=media_real, scale=std_real/np.sqrt(n))
  x = np.linspace(norm_dist.ppf(0.001), norm_dist.ppf(0.999), 100)
  plt.plot(x, norm_dist.pdf(x), 'k-', lw=2, label='Distribuição Normal Teórica')

  # Adicionar legendas aos eixos x e y
  plt.xlabel('Valores')
  plt.ylabel('Densidade de Probabilidade')

  plt.title(titulo)
  plt.legend()
  plt.show()

# Leitura de parâmetros
lambda_exp = float(input("Digite o parâmetro lambda para a Exponencial: "))
a_uniforme = float(input("Digite o parâmetro 'a' para a Uniforme: "))
b_uniforme = float(input("Digite o parâmetro 'b' para a Uniforme: "))
m = int(input("Digite o número de amostras (m): "))
n = int(input("Digite o tamanho das amostras (n): "))

# Gerar amostras e plotar histogramas
AmostrasHistogramas(binom, (n, 0.05), n, m, 'Distribuição Binomial (p < 0.1)')
AmostrasHistogramas(binom, (n, 0.5), n, m, 'Distribuição Binomial (p = 0.5)')
AmostrasHistogramas(expon, (0, 1/lambda_exp), n, m, 'Distribuição Exponencial')
AmostrasHistogramas(uniform, (a_uniforme, b_uniforme), n, m, 'Distribuição Uniforme')