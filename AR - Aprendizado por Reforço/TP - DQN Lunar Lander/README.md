# TP Aprendizado por Reforo - Deep Q-Network no Lunar Lander

Este projeto contém a implementação de um agente Deep Q-Network (DQN) para solucionar o ambiente "LunarLander-v3" da biblioteca Gymnasium. O projeto inclui:
- Implementação manual do DQN (Redes Neurais, Replay Buffer, Epsilon-Greedy).
- Busca de Hiperparâmetros (Grid Search) com 96 configurações.
- Comparação de desempenho com a biblioteca Stable Baselines3.


## 📁 Estrutura do Projeto

```
TP/
├── Documentacao.pdf                            # Documentação técnica do trabalho
├── dqn_grid_search_results.pkl                 # Arquivo de checkpoint com os resultados da busca de hiperparâmetros 
├── README.md                                   # Este arquivo
└── TP_DQN_Vinicius_Trindade_2020007112.ipynb   # Código-fonte principal (Jupyter Notebook)
```

## ⚙️ Ambiente de Desenvolvimento

* Sistema Operacional do computador: Windows 11; 
* Linguagem de programação implementada: Python 3.10.11;
* Processador: i7;
* Quantidade de memória RAM: 16GB.
* GPU: NVIDIA GeForce RTX 2060.

## 🔧 Requisições do projeto

### Bibliotecas 

* gymnasium==1.2.1                 # (com suporte a box2d)
* torch==2.8.0+cu129               # (PyTorch)
* stable-baselines3==2.7.0
* numpy==1.26.4
* pandas==2.2.2
* matplotlib==3.8.2
* tqdm==4.67.1
* pygame==2.6.1
* swig==4.4.0                       # (necessário para compilar o box2d em alguns sistemas)
* box2d==2.3.10

### Instalação

Para instalar todas as dependências necessárias basta rodar as primeiras celulas do notebook.

## ▶️ Execução

A implementação completa está contida no Jupyter Notebook.

- Passo 1: Abra o arquivo do Jupyter Notebook "TP_DQN_Vinicius_Trindade_2020007112.ipynb".

- Passo 2: Execução Sequencial:
   Execute as células sequencialmente (Kernel -> Restart & Run All).

   IMPORTANTE SOBRE O GRID SEARCH:
   - O notebook está configurado para buscar o arquivo 'dqn_grid_search_results.pkl'
     no diretório local.
   - Se este arquivo estiver presente, o código carregará os resultados dos 96
     experimentos pré-calculados, economizando dias de processamento, e pulará
     diretamente para a análise e plotagem dos gráficos.
   - Se desejar rodar o treinamento do zero (reproduzir a busca de hiperparâmetros),
     exclua ou renomeie o arquivo 'dqn_grid_search_results.pkl' antes de executar
     a célula correspondente.

- Passo 3: Comparação com Stable Baselines3:
   - As células finais do notebook treinam um agente SB3 do zero e comparam seu
     desempenho com o melhor modelo da implementação manual. Esta etapa é
     executada em tempo real.