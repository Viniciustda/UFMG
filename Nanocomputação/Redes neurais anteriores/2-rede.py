"""
rede_neural.py
Rede CNN -> MLP para associar imagem -> espectro (regressão).
Coloque este arquivo em: Plasmonica/Código/rede_neural.py

Dependências:
    - python >= 3.8
    - torch, torchvision
    - pillow
    - pandas, numpy, matplotlib
    - scikit-learn
    - (opcional) scipy for Savitzky-Golay smoothing

Exemplo de uso (na pasta Plasmonica):
    python Código/rede_neural.py --data_dir "../Dados - NanoComp/Dataset_Organizado" --epochs 200
"""

import os
import glob
import argparse
from typing import List, Tuple, Optional
import math
import time
import copy

import numpy as np
import pandas as pd
from PIL import Image
import matplotlib.pyplot as plt

import torch
import torch.nn as nn
from torch.utils.data import Dataset, DataLoader, random_split
import torchvision.transforms as T
import torchvision.models as models

from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import KFold

# optional smoothing
try:
    from scipy.signal import savgol_filter
    HAS_SCIPY = True
except Exception:
    HAS_SCIPY = False

# -----------------------
# Utilities for data I/O
# -----------------------

def read_spectrum_csv(path_csv: str) -> Tuple[np.ndarray, np.ndarray]:
    """
    Lê CSV com colunas ['angle','lambda','electric_field_norm'] ou similar.
    Retorna: (wavelengths, spectrum_values) como arrays 1D ordenados por lambda.
    """
    df = pd.read_csv(path_csv)
    # try to find columns
    if 'lambda' not in df.columns or ('electric_field_norm' not in df.columns and 'E' not in df.columns):
        raise ValueError(f"CSV {path_csv} não contém colunas esperadas. Colunas encontradas: {df.columns.tolist()}")
    lambdas = df['lambda'].values
    # assume electric_field_norm column exists
    if 'electric_field_norm' in df.columns:
        vals = df['electric_field_norm'].values
    else:
        # fallback to second numeric column besides lambda
        numeric_cols = df.select_dtypes(include=[np.number]).columns.tolist()
        numeric_cols.remove('lambda')
        vals = df[numeric_cols[0]].values
    # sort by lambda
    sort_idx = np.argsort(lambdas)
    return lambdas[sort_idx], vals[sort_idx]

def find_pairs(data_dir: str) -> List[Tuple[str, str]]:
    """
    Procura por pares de arquivos: Imagem_*.png (ou jpg) e Grafico_*.csv correspondentes.
    Assume que o nome base antes do sufixo é igual, por exemplo:
        Imagem_01_1.png <-> Grafico_01_1.csv
    Retorna lista de (image_path, csv_path).
    """
    # gather images
    img_patterns = ['**/Imagem_*.png', '**/Imagem_*.jpg', '**/imagem_*.png', '**/image_*.png']
    imgs = []
    for p in img_patterns:
        imgs.extend(glob.glob(os.path.join(data_dir, p), recursive=True))
    pairs = []
    for img in imgs:
        base = os.path.splitext(os.path.basename(img))[0]
        # possible csv name
        csv_name1 = os.path.join(os.path.dirname(img), 'Grafico_' + base.split('_', 1)[-1] + '.csv')
        # but easier: try to find csv with same trailing tokens:
        csv_candidates = glob.glob(os.path.join(data_dir, '**', f'{os.path.splitext(os.path.basename(img))[0].replace("Imagem","Grafico")}*.csv'), recursive=True)
        # fallback: try grafico with same suffix
        if csv_candidates:
            csv = csv_candidates[0]
        else:
            # try direct replacement Imagem_XX -> Grafico_XX
            csv = img.replace('Imagem_', 'Grafico_').rsplit('.', 1)[0] + '.csv'
            if not os.path.exists(csv):
                # try other simple heuristic
                csv = img.replace('Imagem', 'Grafico').rsplit('.', 1)[0] + '.csv'
                if not os.path.exists(csv):
                    continue
        pairs.append((img, csv))
    return pairs

# -----------------------
# Dataset
# -----------------------
class SpectrumImageDataset(Dataset):
    def __init__(self, pairs: List[Tuple[str, str]],
                 img_size: Tuple[int,int] = (128,128),
                 augment: bool = True,
                 ):
        self.pairs = pairs
        self.img_size = img_size
        self.augment = augment

        # transforms
        self.transform_train = T.Compose([
            T.Resize(img_size),
            T.RandomRotation(15),
            T.RandomHorizontalFlip(),
            T.RandomVerticalFlip(),
            T.ColorJitter(brightness=0.1, contrast=0.1),
            T.ToTensor(),
        ])
        self.transform_val = T.Compose([
            T.Resize(img_size),
            T.ToTensor(),
        ])

        # load a sample spectrum to infer length
        if len(pairs) == 0:
            raise ValueError("Nenhum par imagem/csv encontrado.")
        _, first_csv = pairs[0]
        _, vals = read_spectrum_csv(first_csv)
        self.spectrum_len = len(vals)

        # prepare scaler (standardize)
        # We'll fit scaler lazily when dataset created externally (caller can call fit_scaler)
        self.scaler = None

    def __len__(self):
        return len(self.pairs)

    def __getitem__(self, idx):
        img_path, csv_path = self.pairs[idx]
        # image
        img = Image.open(img_path).convert('RGB')
        transform = self.transform_train if self.augment else self.transform_val
        img_t = transform(img)

        # spectrum
        _, vals = read_spectrum_csv(csv_path)
        y = np.array(vals, dtype=np.float32)
        if self.scaler is not None:
            y = self.scaler.transform(y.reshape(1, -1)).reshape(-1).astype(np.float32)
        else:
            y = y.astype(np.float32)

        return img_t, y

    def fit_scaler(self):
        """
        Fit scaler over all spectra in the dataset (StandardScaler). Useful if you want the
        targets normalized to zero mean unit variance for training.
        """
        arr = []
        for _, csv_path in self.pairs:
            _, vals = read_spectrum_csv(csv_path)
            arr.append(vals)
        arr = np.stack(arr, axis=0)  # (N_samples, spectrum_len)
        scaler = StandardScaler()
        scaler.fit(arr)
        self.scaler = scaler
        return scaler

# -----------------------
# Model
# -----------------------
class CNNRegressor(nn.Module):
    def __init__(self, output_dim: int, use_resnet: bool = True, pretrained: bool = False):
        super().__init__()
        self.output_dim = output_dim
        if use_resnet:
            # use resnet18 and replace fc
            res = models.resnet18(pretrained=pretrained)
            # replace first conv if input small? keep default
            in_features = res.fc.in_features
            # remove original fc and replace with a linear mapping to the output dim
            res.fc = nn.Identity()
            self.feature_extractor = res
            self.regressor = nn.Sequential(
                nn.Linear(in_features, 512),
                nn.ReLU(),
                nn.Dropout(0.5),
                nn.Linear(512, output_dim)
            )
        else:
            # small custom CNN
            self.feature_extractor = nn.Sequential(
                nn.Conv2d(3, 32, 3, stride=1, padding=1),
                nn.BatchNorm2d(32),
                nn.ReLU(),
                nn.MaxPool2d(2),
                nn.Conv2d(32, 64, 3, stride=1, padding=1),
                nn.BatchNorm2d(64),
                nn.ReLU(),
                nn.MaxPool2d(2),
                nn.Conv2d(64, 128, 3, stride=1, padding=1),
                nn.BatchNorm2d(128),
                nn.ReLU(),
                nn.AdaptiveAvgPool2d((1,1)),
            )
            self.regressor = nn.Sequential(
                nn.Flatten(),
                nn.Linear(128, 256),
                nn.ReLU(),
                nn.Dropout(0.5),
                nn.Linear(256, output_dim),
            )

    def forward(self, x):
        feats = self.feature_extractor(x)
        out = self.regressor(feats.view(feats.size(0), -1))
        return out

# -----------------------
# Training & evaluation
# -----------------------

def train_model(model: nn.Module, dataloaders: dict, criterion, optimizer,
                device, num_epochs=100, patience=20, scheduler=None):
    """
    Standard training loop with early stopping on validation loss.
    dataloaders: {'train':..., 'val':...}
    Returns best_model_wts and history dict.
    """
    since = time.time()
    best_model_wts = copy.deepcopy(model.state_dict())
    best_loss = float('inf')
    epochs_no_improve = 0
    history = {'train_loss': [], 'val_loss': []}

    for epoch in range(num_epochs):
        # each epoch
        for phase in ['train', 'val']:
            if phase == 'train':
                model.train()
            else:
                model.eval()
            running_loss = 0.0
            n_samples = 0
            for inputs, targets in dataloaders[phase]:
                inputs = inputs.to(device)
                targets = targets.to(device)

                optimizer.zero_grad()
                with torch.set_grad_enabled(phase == 'train'):
                    outputs = model(inputs)
                    loss = criterion(outputs, targets)
                    if phase == 'train':
                        loss.backward()
                        optimizer.step()
                running_loss += loss.item() * inputs.size(0)
                n_samples += inputs.size(0)
            epoch_loss = running_loss / n_samples
            history[f'{phase}_loss'].append(epoch_loss)
            if phase == 'val':
                if epoch_loss < best_loss - 1e-8:
                    best_loss = epoch_loss
                    best_model_wts = copy.deepcopy(model.state_dict())
                    epochs_no_improve = 0
                else:
                    epochs_no_improve += 1

        if scheduler is not None:
            scheduler.step()

        # simple early stopping
        if epochs_no_improve >= patience:
            print(f"Early stopping at epoch {epoch+1}. Best val loss: {best_loss:.6e}")
            break
        if (epoch+1) % 10 == 0 or epoch == 0:
            print(f"Epoch {epoch+1}/{num_epochs} | train_loss {history['train_loss'][-1]:.4e} | val_loss {history['val_loss'][-1]:.4e}")

    time_elapsed = time.time() - since
    print(f"Training complete in {time_elapsed//60:.0f}m {time_elapsed%60:.0f}s. Best val loss: {best_loss:.6e}")
    model.load_state_dict(best_model_wts)
    return model, history

def evaluate_model(model: nn.Module, dataloader: DataLoader, device):
    model.eval()
    losses = []
    preds = []
    trues = []
    with torch.no_grad():
        for inputs, targets in dataloader:
            inputs = inputs.to(device)
            targets = targets.to(device)
            outputs = model(inputs)
            preds.append(outputs.cpu().numpy())
            trues.append(targets.cpu().numpy())
    preds = np.concatenate(preds, axis=0)
    trues = np.concatenate(trues, axis=0)
    mse = np.mean((preds - trues)**2)
    return mse, preds, trues

# -----------------------
# Plotting / prediction
# -----------------------
def inverse_transform_spectrum(spectrum: np.ndarray, scaler: Optional[StandardScaler]):
    if scaler is None:
        return spectrum
    return scaler.inverse_transform(spectrum.reshape(1, -1)).reshape(-1)

def smooth_spectrum(y: np.ndarray, window_length: int = 9, polyorder: int = 3):
    if HAS_SCIPY:
        # ensure odd window
        if window_length % 2 == 0:
            window_length += 1
        window_length = min(window_length, len(y)-1 if (len(y)-1)%2==1 else len(y))
        try:
            return savgol_filter(y, window_length, polyorder)
        except Exception:
            return y
    else:
        # fallback: simple moving average
        kernel = np.ones(5)/5.0
        return np.convolve(y, kernel, mode='same')

def predict_and_plot(model: nn.Module, img_path: str, csv_path: str, scaler: Optional[StandardScaler],
                     device, smooth: bool = True, savepath: Optional[str] = None):
    lambdas, y_true = read_spectrum_csv(csv_path)
    # prepare image
    img = Image.open(img_path).convert('RGB')
    transform = T.Compose([T.Resize((128,128)), T.ToTensor()])
    x = transform(img).unsqueeze(0).to(device)
    model.eval()
    with torch.no_grad():
        y_pred = model(x).cpu().numpy().reshape(-1)
    y_pred_inv = inverse_transform_spectrum(y_pred, scaler)
    y_true_inv = y_true
    if smooth:
        y_pred_inv = smooth_spectrum(y_pred_inv)
    # plot
    fig, axs = plt.subplots(1,2, figsize=(10,4))
    axs[0].imshow(img)
    axs[0].axis('off')
    axs[0].set_title(os.path.basename(img_path))
    axs[1].plot(lambdas, y_true_inv, label='True')
    axs[1].plot(lambdas, y_pred_inv, label='Predicted')
    axs[1].set_xlabel('lambda')
    axs[1].set_ylabel('electric_field_norm')
    axs[1].legend()
    plt.tight_layout()
    if savepath:
        plt.savefig(savepath, dpi=150)
    plt.show()
    return lambdas, y_true_inv, y_pred_inv

# -----------------------
# Helpers to save/load model & scaler
# -----------------------
def save_checkpoint(model: nn.Module, scaler: Optional[StandardScaler], path: str):
    payload = {'model_state': model.state_dict()}
    if scaler is not None:
        payload['scaler_mean'] = scaler.mean_.tolist()
        payload['scaler_scale'] = scaler.scale_.tolist()
    torch.save(payload, path)
    print(f"Salvo checkpoint em {path}")

def load_checkpoint(model: nn.Module, path: str):
    ckpt = torch.load(path, map_location='cpu')
    model.load_state_dict(ckpt['model_state'])
    scaler = None
    if 'scaler_mean' in ckpt and 'scaler_scale' in ckpt:
        scaler = StandardScaler()
        scaler.mean_ = np.array(ckpt['scaler_mean'])
        scaler.scale_ = np.array(ckpt['scaler_scale'])
    return model, scaler

# -----------------------
# Main procedure
# -----------------------
def main(args):
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    print("Device:", device)

    pairs = find_pairs(args.data_dir)
    if len(pairs) == 0:
        raise RuntimeError("Nenhum par imagem/csv encontrado. Verifique data_dir e nomes dos arquivos.")
    print(f"Encontrados {len(pairs)} pares (image, csv). Usando {args.max_samples} primeiros (se aplicável).")
    if args.max_samples is not None:
        pairs = pairs[:args.max_samples]

    dataset = SpectrumImageDataset(pairs, img_size=(args.img_size,args.img_size), augment=True)
    scaler = dataset.fit_scaler() if args.normalize_targets else None
    dataset.scaler = scaler  # ensure dataset uses scaler

    # split train/val/test
    n = len(dataset)
    n_val = max(1, int(n * args.val_fraction))
    n_test = max(1, int(n * args.test_fraction))
    n_train = n - n_val - n_test
    if n_train < 1:
        n_train = max(1, n - n_val - n_test)
    lengths = [n_train, n_val, n_test]
    # adjust if sum mismatch
    if sum(lengths) != n:
        lengths[0] = n - lengths[1] - lengths[2]
    train_ds, val_ds, test_ds = random_split(dataset, lengths)
    # set augmentation off for val/test
    val_ds.dataset.augment = False
    test_ds.dataset.augment = False

    train_loader = DataLoader(train_ds, batch_size=args.batch_size, shuffle=True, num_workers=0)
    val_loader = DataLoader(val_ds, batch_size=args.batch_size, shuffle=False, num_workers=0)
    test_loader = DataLoader(test_ds, batch_size=args.batch_size, shuffle=False, num_workers=0)
    dataloaders = {'train': train_loader, 'val': val_loader}

    out_dim = dataset.spectrum_len
    print("Spectrum length inferred:", out_dim)

    model = CNNRegressor(output_dim=out_dim, use_resnet=args.use_resnet, pretrained=args.pretrained).to(device)
    criterion = nn.MSELoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=args.lr, weight_decay=args.weight_decay)
    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimizer, mode='min', factor=0.5, patience=10) if args.use_scheduler else None

    model, history = train_model(model, dataloaders, criterion, optimizer, device,
                                 num_epochs=args.epochs, patience=args.patience, scheduler=None)

    # evaluate on test
    test_mse, preds, trues = evaluate_model(model, test_loader, device)
    print(f"Test MSE: {test_mse:.6e}")

    # save model + scaler
    os.makedirs(args.save_dir, exist_ok=True)
    ckpt_path = os.path.join(args.save_dir, 'model_checkpoint.pth')
    save_checkpoint(model, scaler, ckpt_path)

    # show prediction on first test sample
    img_path, csv_path = test_ds[0][0], None
    # but test_ds[0] returns tensors; instead grab from pairs
    test_pair = pairs[n_train + 0] if n_train + 0 < len(pairs) else pairs[-1]
    predict_and_plot(model, test_pair[0], test_pair[1], scaler, device, smooth=True, savepath=os.path.join(args.save_dir, 'example_pred.png'))

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--data_dir', type=str, default='../Dados - NanoComp/Dataset_Organizado', help='pasta com Imagem_*.png e Grafico_*.csv')
    parser.add_argument('--save_dir', type=str, default='.', help='onde salvar checkpoints e imagens')
    parser.add_argument('--img_size', type=int, default=128)
    parser.add_argument('--batch_size', type=int, default=8)
    parser.add_argument('--epochs', type=int, default=200)
    parser.add_argument('--lr', type=float, default=1e-4)
    parser.add_argument('--weight_decay', type=float, default=1e-5)
    parser.add_argument('--val_fraction', type=float, default=0.2)
    parser.add_argument('--test_fraction', type=float, default=0.1)
    parser.add_argument('--patience', type=int, default=30)
    parser.add_argument('--use_resnet', action='store_true', help='usar ResNet18 como backbone (recomendado)')
    parser.add_argument('--pretrained', action='store_true', help='usar pesos pretrained (pode baixar da internet)')
    parser.add_argument('--normalize_targets', action='store_true', help='normalizar espectros com StandardScaler')
    parser.add_argument('--max_samples', type=int, default=None)
    parser.add_argument('--use_scheduler', action='store_true', help='usar ReduceLROnPlateau scheduler')
    args = parser.parse_args()
    main(args)