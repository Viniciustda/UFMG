import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
import os

def create_model(img_height, img_width, num_spectrum_points):
    """
    Cria um modelo inspirado no artigo de Sajedian et al. (2019),
    combinando uma ResNet-CNN com uma RNN (GRU), mas simplificado
    para um conjunto de dados menor.
    
    Argumentos:
    img_height (int): A altura padrão das imagens de entrada.
    img_width (int): A largura padrão das imagens de entrada.
    num_spectrum_points (int): O número de pontos padrão para o espectro de saída.
    """
    
    input_shape = (img_height, img_width, 1)
    image_input = layers.Input(shape=input_shape, name="input_image")
    
    # --- PARTE 1: A CNN (ResNet Simplificada) ---
    
    # Bloco Inicial
    # padding='same' garante que o tamanho da saída seja o mesmo
    # (ou quase, se a divisão não for exata)
    x = layers.Conv2D(64, (7, 7), padding='same', activation=None)(image_input)
    x = layers.BatchNormalization()(x)
    x = layers.LeakyReLU()(x)
    # Após Pool1: (308, 310) -> (154, 155)
    x = layers.MaxPooling2D((2, 2))(x)
    
    # Bloco Residual 1
    shortcut = x
    x = layers.Conv2D(64, (3, 3), padding='same', activation=None)(x)
    x = layers.BatchNormalization()(x)
    x = layers.LeakyReLU()(x)
    x = layers.Conv2D(64, (3, 3), padding='same', activation=None)(x)
    x = layers.BatchNormalization()(x)
    
    x = layers.Add()([x, shortcut]) # Conexão residual
    x = layers.LeakyReLU()(x)
    
    # Bloco Residual 2
    # Ajusta dimensão do atalho
    shortcut = layers.Conv2D(128, (1, 1), padding='same')(x) 
    x = layers.Conv2D(128, (3, 3), padding='same', activation=None)(x)
    x = layers.BatchNormalization()(x)
    x = layers.LeakyReLU()(x)
    
    x = layers.Add()([x, shortcut])
    x = layers.LeakyReLU()(x)
    
    # Adicionando mais um Pooling para reduzir mais os dados antes da RNN
    # Após Pool2: (154, 155) -> (77, 78) (arredondando para cima)
    x = layers.MaxPooling2D((2, 2))(x)

    # --- PARTE 2: Transição da CNN para a RNN ---
    # A RNN vai "ler" a imagem achatada como uma sequência.
    
    # CORREÇÃO:
    # O 'x' tem shape (batch, H, W, C). 
    # Queremos (batch, H*W, C).
    # O target_shape do Reshape não inclui o batch. 
    # Usamos -1 para H*W e pegamos o C da última dimensão do shape.
    
    # O shape estático de x é (None, 77, 78, 128)
    # x.shape[-1] é 128. O -1 infere o 77*78=5928.
    rnn_input = layers.Reshape((-1, x.shape[-1]))(x)

    # --- PARTE 3: A RNN (GRU) ---
    # GRU encontra relações na sequência de features. O artigo usa GRU[cite: 80].
    gru_output = layers.GRU(64)(rnn_input)
    
    # --- PARTE 4: Cabeça de Regressão ---
    x = layers.Dense(1024, activation='relu')(gru_output)
    x = layers.Dropout(0.4)(x)
    
    x = layers.Dense(512, activation='relu')(x)
    x = layers.Dropout(0.4)(x)
    
    # Camada de Saída
    spectrum_output = layers.Dense(num_spectrum_points, activation='linear', name='output_spectrum')(x)

    # Construir e compilar o modelo
    model = keras.Model(inputs=image_input, outputs=spectrum_output)
    
    # Otimizador e loss do artigo
    optimizer = keras.optimizers.Adam(learning_rate=0.0001) # [cite: 266]
    loss = 'mean_squared_error' # MSE [cite: 187, 188]
    
    model.compile(optimizer=optimizer, loss=loss)
    
    return model

# --- Callback de Parada Antecipada ---
early_stopping = keras.callbacks.EarlyStopping(
    monitor="val_loss",
    patience=30,
    restore_best_weights=True,
    verbose=1
)

# --- Importando o script de preparação de dados ---
from preparar_dados import load_and_prepare_data

# --- COMO USAR ---

# 1. Defina seus parâmetros
IMG_H = 308
IMG_W = 310
# O número de pontos foi ajustado para corresponder aos dados de saída
SPEC_POINTS = 101 # Cada espectro nos arquivos de saída possui 101 pontos.

# 2. Crie o modelo
model = create_model(IMG_H, IMG_W, SPEC_POINTS)

# 3. Imprima a arquitetura para verificar
print("Arquitetura do Modelo Criada:")
model.summary()

# 4. Carregue e prepare os dados
print("\n--- Loading and Preparing Data ---")
DATASET_PATH = os.path.join('..', 'Dados - NanoComp', 'Dataset_Organizado')
X_train, X_test, y_train, y_test = load_and_prepare_data(
    dataset_path=DATASET_PATH,
    img_height=IMG_H,
    img_width=IMG_W
)

# 5. Treine o modelo (se os dados foram carregados corretamente)
if X_train is not None:
    print("\n--- Starting Model Training ---")
    
    history = model.fit(
        X_train,
        y_train,
        epochs=200,  # Aumente o número de épocas conforme necessário
        batch_size=16, # Um batch size menor pode ajudar com memória limitada
        validation_data=(X_test, y_test),
        callbacks=[early_stopping] # Usando o EarlyStopping definido anteriormente
    )
    
    print("\n--- Training Finished ---")

    # 6. Avalie o modelo
    print("\n--- Evaluating Model on Test Set ---")
    test_loss = model.evaluate(X_test, y_test)
    print(f"Loss (MSE) on test set: {test_loss}")

    # 7. Salve o modelo treinado
    model.save("nanoparticle_model.h5")
    print("\nModel saved as 'nanoparticle_model.h5'")
else:
    print("\nTraining not started due to an error in data loading.")