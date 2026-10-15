from rede import *
import torch

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

model = CNNRegressor(output_dim=101, use_resnet=True)
model, scaler = load_checkpoint(model, "Resultados/model_checkpoint.pth")
model = model.to(device)

img_path = "Imagem_12_6.png"
csv_path = "Grafico_12_6.csv"

predict_and_plot(model, img_path, csv_path, scaler, device)
