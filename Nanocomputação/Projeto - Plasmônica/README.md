# Projeto: Predição de Propriedades Ópticas de Estruturas Plasmônias Usando Redes Neurais Profundas​

## Grupo
- Vinicius Trindade Dias Abel
- Guilherme Buxbaum Marinho Guerra
- Fabiano Costa Santana

---

## Objetivo

Desenvolver um modelo de **Deep Learning** capaz de prever a resposta óptica (espectro de absorção/campo elétrico) de nanoestruturas a partir de suas **geometrias 2D**.
O objetivo é substituir ou acelerar simulações numéricas custosas (como FDTD ou FEM) por um modelo neural rápido e preciso.

---

# Pipeline Completo

## 1. Entrada dos Dados

Os dados originais provêm de simulações e definições geométricas:

### 1.1 Geometrias (`Dados - NanoComp/Imput_XX.txt`)
Arquivos de texto contendo as coordenadas dos polígonos que definem a nanoestrutura.

### 1.2 Espectros (`Dados - NanoComp/Output_XX.txt`)
Arquivos contendo os resultados da simulação para diferentes ângulos de incidência.
- **Colunas**: Ângulo, Comprimento de onda ($\lambda$), Norma do Campo Elétrico.

---

## 2. Organização do Dataset (`Dataset_Organizado`)

Para o treinamento, os dados foram processados e pareados em:

1.  **Imagens (`Imagem_XX_Y.png`)**: Representação visual da geometria (100x100 px).
2.  **Curvas (`Grafico_XX_Y.csv`)**: Arquivos CSV contendo o espectro alvo correspondente.
    - Colunas usadas: `lambda` (eixo X) e `electric_field_norm` (eixo Y).

O pareamento é feito automaticamente pelo script de treinamento, associando cada imagem ao seu respectivo gráfico.

---

## 3. Modelo de Deep Learning (`Code/NeuralNetwork.py`)

O núcleo do projeto é uma rede neural híbrida **CNN-GRU**, inspirada no estado da arte para esse tipo de problema (ex: *Sajedian et al. 2019*).

### Arquitetura

O modelo processa a imagem para extrair características espaciais e mapeá-las para uma sequência espectral.

1.  **CNN (Extrator de Características)**:
    - Camadas Convolucionais para detectar bordas e formas da nanoestrutura.
    - *Pooling* para redução de dimensionalidade.
    - **Versão Original**: 12 blocos convolucionais com conexões residuais (skip connections).
    - **Versão Reduzida**: 3 blocos convolucionais (otimizada para datasets menores).

2.  **GRU (Gated Recurrent Unit)**:
    - Processa os mapas de características como uma sequência.
    - Ajuda a modelar a correlação entre pontos adjacentes no espectro.

3.  **MLP (Camadas Densas)**:
    - Camadas `Linear` finais para projetar a saída no tamanho exato do espectro (ex: 101 pontos).

### Decisões de Projeto e Parâmetros

| Parâmetro | Valor / Decisão | Justificativa |
| :--- | :--- | :--- |
| **Input** | Imagens 100x100 (Grayscale ou RGB) | Resolução suficiente para captar a geometria sem custo computacional excessivo. |
| **Output** | Vetor de tamanho variável (ex: 101) | O modelo se adapta automaticamente ao número de pontos do CSV. |
| **Loss Function** | **MSE** (Mean Squared Error) | Penaliza erros grandes, ideal para regressão de curvas contínuas. |
| **Otimizador** | **NAdam** | Variação do Adam com momento de Nesterov, geralmente converge mais rápido. |
| **Normalização** | `StandardScaler` (sklearn) | Normaliza os valores alvo (espectros) para média 0 e desvio 1, facilitando a convergência. |
| **Data Augmentation** | Desativada no estado atual (somente resize + grayscale/RGB) | Mantém avaliação estável; o código já está preparado para habilitar augmentação real no treino sem afetar val/test. |
| **Split** | 80% Treino / 10% Val / 10% Teste | Prioriza o volume de treino, crucial para datasets pequenos, mantendo o mínimo para validação. |

---

## 4. Scripts Principais (`Code/`)

Esta pasta tem 4 scripts principais. Cada um resolve uma etapa diferente do fluxo.

### 4.1 Processamento dos arquivos de simulação (`ProcessFiles.py`)

**O que faz:**
- Lê `Imput_XX.txt` (geometria) e `Output_XX.txt` (espectro simulado).
- Gera pares imagem+csv prontos para treino/teste.

**Entrada esperada:**
- Arquivos em `Simulation_Input/` com padrão `Imput_XX.txt` e `Output_XX.txt`.

**Saída gerada:**
- `Imagem_XX_Y.png` (100x100, forma geométrica).
- `Grafico_XX_Y.csv` (colunas: `angle`, `lambda`, `electric_field_norm`).

**Quando usar:**
- Quando você recebeu novos `.txt` de simulação e quer convertê-los para o formato usado pelos outros scripts.

```bash
python Code/ProcessFiles.py
```

### 4.2 Treino/validação/teste do modelo (`NeuralNetwork.py`)

**O que faz:**
- Encontra pares `Imagem_*.png/jpg` + `Grafico_*.csv` no `--data_dir`.
- Faz split reprodutível **80/10/10** (train/val/test) com `--seed`.
- Ajusta `StandardScaler` **somente no treino**.
- Treina modelo `reduced` ou `original`.
- Salva checkpoints, métricas e gráficos de treino/teste.

**Principais saídas em `--save_dir`:**
- `best_by_val.pth`
- `best_by_test.pth` (alias de compatibilidade)
- `final_checkpoint.pth`
- `split_manifest.json`
- `test_results.npz`
- `loss_history.png`
- `test_prediction_*.png`

```bash
python Code/NeuralNetwork.py \
    --data_dir "Dados - NanoComp/Graficos" \
    --save_dir "Article_Results_Training" \
    --model reduced \
    --epochs 500 \
    --normalize_targets \
    --patience 50 \
    --seed 33
```

### 4.3 Avaliação com Ground Truth (`TestModel.py`)

**O que faz:**
- Carrega checkpoint treinado.
- Compara predição da rede com o CSV real (curva verdadeira).
- Usa a mesma lógica de visualização do treino.

**Modos:**
- Automático aleatório: `--data_dir`
- Manual: `--img` + `--csv`
- Lote completo: `--data_dir --all_pairs --output_dir --no_show`

**Observação importante:**
- Para escala correta do espectro previsto, prefira `final_checkpoint.pth` (ou outro checkpoint que contenha scaler).

```bash
python Code/TestModel.py \
    --checkpoint "Article_Results_Training/final_checkpoint.pth" \
    --data_dir "Simulation_Input" \
    --all_pairs \
    --output_dir "Simulation_Output" \
    --no_show
```

### 4.4 Inferência sem CSV (`UseModel.py`)

**O que faz:**
- Prevê espectro a partir de uma nova imagem **sem** CSV de referência.
- Salva gráfico da previsão.

**Entrada obrigatória:**
- `--img`: imagem nova.
- `--checkpoint`: modelo treinado.
- `--points`: número de pontos de saída do modelo (deve bater com o treino).

**Observação:**
- Como não existe CSV, o eixo X (comprimento de onda) é estimado para visualização.

```bash
python Code/UseModel.py \
    --checkpoint "Article_Results_Training/final_checkpoint.pth" \
    --img "Caminho/Para/Nova_Imagem.png" \
    --model reduced \
    --points 101 \
    --save_plot "Forecast.png"
```

### Resumo rápido: qual script usar?

- Quero converter `Imput/Output` em `Imagem/Grafico`: **`ProcessFiles.py`**
- Quero treinar e validar modelo: **`NeuralNetwork.py`**
- Quero comparar previsão com CSV real: **`TestModel.py`**
- Quero prever em imagem sem CSV: **`UseModel.py`**

---

## 5. Resultados

Os resultados dos experimentos são salvos nas pastas:
- `Article_Results_Training/`: Contém os modelos salvos (`.pth`) e logs de treinamento.
- `Article_Results_Test/`: Contém exemplos e artefatos de comparação em testes manuais.
- `Simulation_Output/`: Contém gráficos comparativos gerados em lote com `TestModel.py`.

O modelo "Reduced" demonstrou ser capaz de aprender a correlação entre a geometria e o espectro óptico, convergindo adequadamente mesmo com dataset limitado, com apoio de split reprodutível, normalização dos alvos e *Early Stopping*.