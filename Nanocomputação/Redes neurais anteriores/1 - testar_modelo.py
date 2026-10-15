import os
import numpy as np
import matplotlib.pyplot as plt
from tensorflow import keras
from preparar_dados import load_and_prepare_data

# --- 1. Definição dos Parâmetros (Devem ser IDÊNTICOS aos do treino) ---
IMG_H = 308
IMG_W = 310
DATASET_PATH = os.path.join('..', 'Dados - NanoComp', 'Dataset_Organizado')
MODEL_PATH = 'nanoparticle_model.h5'
NUM_TEST_SAMPLES_TO_SHOW = 5 # Quantos exemplos aleatórios queremos ver

# --- 2. Carregar o Modelo Treinado ---
print(f"Carregando modelo de '{MODEL_PATH}'...")
try:
    model = keras.models.load_model(MODEL_PATH)
    model.summary()
except Exception as e:
    print(f"Erro ao carregar o modelo. Verifique o caminho: {e}")
    exit()

# --- 3. Carregar os Dados de Teste ---
# Nós re-executamos a função, mas só nos importamos com X_test e y_test
print("Carregando dados de teste...")
_, X_test, _, y_test = load_and_prepare_data(
    dataset_path=DATASET_PATH,
    img_height=IMG_H,
    img_width=IMG_W
)

if X_test is None:
    print("Não foi possível carregar os dados de teste.")
    exit()

print(f"{len(X_test)} amostras de teste carregadas.")

# --- 4. Fazer Previsões no Conjunto de Teste ---
print("Fazendo previsões no conjunto de teste...")
y_pred = model.predict(X_test)
print("Previsões concluídas.")

# --- 5. Comparar e Plotar os Resultados ---
print(f"Mostrando {NUM_TEST_SAMPLES_TO_SHOW} comparações aleatórias...")

# Pega N índices aleatórios do nosso conjunto de teste
indices_para_testar = np.random.choice(len(X_test), NUM_TEST_SAMPLES_TO_SHOW, replace=False)

for idx in indices_para_testar:
    # Pega os dados
    img = X_test[idx]
    espectro_real = y_test[idx]
    espectro_previsto = y_pred[idx]

    # Cria a figura para o plot
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))

    # Plot 1: A Imagem de Entrada
    # .reshape() remove a dimensão do canal (de 308, 310, 1 para 308, 310)
    ax1.imshow(img.reshape(IMG_H, IMG_W), cmap='gray')
    ax1.set_title(f"Imagem de Teste #{idx}")
    ax1.axis('off')

    # Plot 2: A Comparação dos Espectros
    ax2.plot(espectro_real, label='Espectro REAL (Correto)', color='blue', linewidth=2)
    ax2.plot(espectro_previsto, label='Espectro PREVISTO (Modelo)', color='red', linestyle='--')
    ax2.set_title("Comparação do Espectro")
    ax2.set_xlabel("Ponto do Espectro (0-100)")
    ax2.set_ylabel("Intensidade")
    ax2.legend()
    ax2.grid(True)
    
    # Mostra o gráfico
    plt.tight_layout()
    plt.show()

print("Fim dos testes.")