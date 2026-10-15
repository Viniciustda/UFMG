import os
import numpy as np
import pandas as pd
from PIL import Image
from sklearn.model_selection import train_test_split
import tensorflow as tf

def load_and_prepare_data(dataset_path, img_height, img_width):
    """
    Carrega as imagens e espectros (.csv), processa-os e os divide 
    em conjuntos de treino e teste.
    """
    
    images = []
    spectrums = []
    
    print(f"Searching for image/spectrum pairs in: {dataset_path}")

    # --- MUDANÇA 1: Procurando por .csv ---
    spectrum_files = [f for f in os.listdir(dataset_path) if f.startswith('Grafico_') and f.endswith('.csv')]

    if not spectrum_files:
        print("No spectrum files (Grafico_*.csv) were found.")
        return None, None, None, None

    for csv_file in spectrum_files: # Renomeei a variável
        # Deriva o nome do arquivo de imagem correspondente
        parts = csv_file.replace('Grafico_', '').replace('.csv', '') # --- MUDANÇA 2 ---
        img_file = f"Imagem_{parts}.png"
        
        full_img_path = os.path.join(dataset_path, img_file)
        full_txt_path = os.path.join(dataset_path, csv_file) # Renomeei a variável
        
        if os.path.exists(full_img_path):
            try:
                # 1. Carregar e processar a imagem
                img = Image.open(full_img_path).convert('L') 
                img = img.resize((img_width, img_height))
                img_array = np.array(img)
                img_array = np.expand_dims(img_array, axis=-1)
                images.append(img_array)
                
                # 2. Carregar o espectro
                # --- MUDANÇA 3: Lendo CSV (separador vírgula é o padrão) ---
                df = pd.read_csv(full_txt_path) 
                
                # --- MUDANÇA 4: Usando o nome da coluna que encontramos ---
                spectrum = df['electric_field_norm'].values 
                
                spectrums.append(spectrum)
                
            except Exception as e:
                print(f"Error processing pair {img_file}/{csv_file}: {e}")
        else:
            print(f"Warning: Image '{img_file}' corresponding to '{csv_file}' was not found.")

    if not images:
        print("No valid data pairs were loaded.")
        return None, None, None, None

    print(f"\n{len(images)} image/spectrum pairs loaded successfully.")

    # Converter listas para arrays numpy
    X = np.array(images, dtype=np.float32)
    y = np.array(spectrums, dtype=np.float32)

    # Normalizar os dados da imagem para o intervalo [0, 1]
    X = X / 255.0

    # --- NOVA LINHA: ESCALAR A SAÍDA 'y' ---
    # Multiplicamos por um número grande para
    # transformar e-14 em números "normais"
    # (Use 1e14, 1e15, 1e16... o que fizer sentido)
    y = y * 1e14
    print(f"\nValores de 'y' (espectros) foram multiplicados por 1e14 para a escala.")
    # ----------------------------------------

    # Dividir os dados em conjuntos de treino e teste
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42
    )
    
    print(f"Data split into:")
    print(f" - Training: {X_train.shape[0]} samples")
    print(f" - Test:  {X_test.shape[0]} samples")
    
    return X_train, X_test, y_train, y_test