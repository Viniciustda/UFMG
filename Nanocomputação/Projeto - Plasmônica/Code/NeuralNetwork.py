"""
NeuralNetwork.py

Versions: 'original' (model inspired by the article Sajedian et al. 2019)
          'reduced'  (smaller version for small datasets)

Save in: Plasmonica/Code/NeuralNetwork.py

Usage (example):
    cd Plasmonica
    python Code/NeuralNetwork.py \
        --data_dir "Dados - NanoComp/Graficos" \
        --save_dir "Article_Results_Training" \
        --model reduced \
        --epochs 500 \
        --normalize_targets \
        --patience 50

Notes:
 - The loader automatically converts RGB images to grayscale (1 channel) by default,
   because the article uses B/W images. Use --keep_rgb to maintain 3 channels.
 - The code automatically infers the spectrum size (number of points in the CSV).
 - For small datasets (e.g., 52 samples) I recommend using --model reduced and --pretrained False.
"""

import json
import os
import glob
import argparse
import time
import copy
import random
from typing import List, Tuple, Optional

import numpy as np
import pandas as pd
from PIL import Image
import matplotlib.pyplot as plt

import torch
import torch.nn as nn
from torch.utils.data import Dataset, DataLoader, Subset
import torchvision.transforms as T

from sklearn.preprocessing import StandardScaler

# -----------------------
# Utilities: CSV + pairs
# -----------------------
def read_spectrum_csv(path_csv: str) -> Tuple[np.ndarray, np.ndarray]:
    df = pd.read_csv(path_csv)
    # expected names in your CSV: 'lambda' and 'electric_field_norm'
    if 'lambda' in df.columns:
        lambdas = df['lambda'].values
    else:
        # fallback first numeric column
        lambdas = df.select_dtypes(include=[np.number]).iloc[:,0].values
    if 'electric_field_norm' in df.columns:
        vals = df['electric_field_norm'].values
    else:
        # fallback to second numeric column if present
        nums = df.select_dtypes(include=[np.number]).columns.tolist()
        if len(nums) >= 2:
            vals = df[nums[1]].values
        else:
            raise ValueError(f"CSV {path_csv} does not have the expected value column.")
    # sort by lambda
    idx = np.argsort(lambdas)
    return lambdas[idx], vals[idx]

def find_pairs(data_dir: str) -> List[Tuple[str,str]]:
    """
    Searches for pairs Image_*.png/jpg and Graph_*.csv in the same folder (or subfolders).
    Heuristic: replaces 'Imagem' with 'Grafico' in the base name if necessary.
    """
    imgs = glob.glob(os.path.join(data_dir, '**/Imagem_*.png'), recursive=True)
    imgs += glob.glob(os.path.join(data_dir, '**/Imagem_*.jpg'), recursive=True)
    pairs = []
    for img in imgs:
        base = os.path.basename(img)
        # try direct replacement (Assuming files are named Imagem... and Grafico...)
        # If your files are named "Image..." and "Graph...", change these strings below.
        csv_guess = os.path.join(os.path.dirname(img), base.replace('Imagem', 'Grafico').rsplit('.',1)[0] + '.csv')
        if os.path.exists(csv_guess):
            pairs.append((img, csv_guess))
            continue
        # also try Grafico_XX_X.csv where XX_X matches trailing part
        suffix = base.split('_',1)[-1].rsplit('.',1)[0]
        alt = os.path.join(os.path.dirname(img), f'Grafico_{suffix}.csv')
        if os.path.exists(alt):
            pairs.append((img, alt))
            continue
        # fallback: search any csv with same trailing tokens
        cands = glob.glob(os.path.join(data_dir, '**', f'*{suffix}*.csv'), recursive=True)
        if cands:
            pairs.append((img, cands[0]))
    pairs.sort(key=lambda p: p[0])
    return pairs

def set_global_seed(seed: int):
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)
    if torch.cuda.is_available():
        torch.cuda.manual_seed_all(seed)

def save_split_manifest(save_dir: str, data_dir: str, seed: int,
                        pairs: List[Tuple[str, str]],
                        train_indices: List[int],
                        val_indices: List[int],
                        test_indices: List[int]):
    data_dir_abs = os.path.abspath(data_dir)

    def _to_rel(path: str) -> str:
        try:
            return os.path.relpath(path, start=data_dir_abs)
        except ValueError:
            return path

    def _records(indices: List[int]):
        recs = []
        for idx in sorted(indices):
            img_path, csv_path = pairs[idx]
            recs.append({
                'index': int(idx),
                'img': _to_rel(img_path),
                'csv': _to_rel(csv_path),
            })
        return recs

    manifest = {
        'seed': int(seed),
        'data_dir': data_dir_abs,
        'counts': {
            'train': len(train_indices),
            'val': len(val_indices),
            'test': len(test_indices),
        },
        'splits': {
            'train': _records(train_indices),
            'val': _records(val_indices),
            'test': _records(test_indices),
        }
    }

    os.makedirs(save_dir, exist_ok=True)
    manifest_path = os.path.join(save_dir, 'split_manifest.json')
    with open(manifest_path, 'w', encoding='utf-8') as f:
        json.dump(manifest, f, indent=2, ensure_ascii=True)
    print("Saved split manifest to", manifest_path)

# -----------------------
# Dataset
# -----------------------
class SpectrumImageDataset(Dataset):
    def __init__(self, pairs: List[Tuple[str,str]],
                 img_size: int = 100,
                 keep_rgb: bool = False,
                 augment: bool = True,
                 normalize_targets: bool = True):
        if len(pairs) == 0:
            raise ValueError("No image/csv pairs found.")
        self.pairs = pairs
        self.img_size = img_size
        self.keep_rgb = keep_rgb
        self.augment = augment
        self.normalize_targets = normalize_targets

        # transforms
        if keep_rgb:
            c = 3
            base_transforms = [
                T.Resize((img_size,img_size)),
            ]
        else:
            c = 1
            base_transforms = [
                T.Resize((img_size,img_size)),
                T.Grayscale(num_output_channels=1),
            ]
        self.transform_train = T.Compose(base_transforms + [
            T.ToTensor(),
        ])
        self.transform_val = T.Compose(base_transforms + [
            T.ToTensor(),
        ])

        # infer spectrum length from first csv
        _, first_csv = pairs[0]
        _, vals = read_spectrum_csv(first_csv)
        self.spectrum_len = len(vals)

        # scaler (fit later)
        self.scaler: Optional[StandardScaler] = None

    def fit_scaler(self, selected_pairs: Optional[List[Tuple[str, str]]] = None):
        source_pairs = self.pairs if selected_pairs is None else selected_pairs
        if len(source_pairs) == 0:
            raise ValueError("Cannot fit scaler with an empty pair list.")
        arr = []
        for _, csv in source_pairs:
            _, vals = read_spectrum_csv(csv)
            arr.append(vals)
        arr = np.stack(arr, axis=0)
        scaler = StandardScaler()
        scaler.fit(arr)
        self.scaler = scaler
        return scaler

    def __len__(self):
        return len(self.pairs)

    def __getitem__(self, idx):
        img_path, csv_path = self.pairs[idx]
        # load image
        img = Image.open(img_path).convert('RGB')  # convert to ensure consistent
        transform = self.transform_train if self.augment else self.transform_val
        if not self.keep_rgb:
            # transform includes Grayscale conversion
            x = transform(img)
        else:
            x = transform(img)
        # load spectrum
        _, vals = read_spectrum_csv(csv_path)
        y = np.array(vals, dtype=np.float32)
        if self.scaler is not None and self.normalize_targets:
            y = self.scaler.transform(y.reshape(1,-1)).reshape(-1).astype(np.float32)
        return x, y

# -----------------------
# Models
# -----------------------
# Helper conv block
def conv_block(in_ch, out_ch, kernel=3, stride=1, padding=1, leaky_slope=0.1):
    return nn.Sequential(
        nn.Conv2d(in_ch, out_ch, kernel_size=kernel, stride=stride, padding=padding, bias=False),
        nn.BatchNorm2d(out_ch),
        nn.LeakyReLU(negative_slope=leaky_slope, inplace=True)
    )

class CNN_GRU_Original(nn.Module):
    """
    Approximate implementation of the architecture from the paper:
    - Multiple conv blocks with 3 shortcuts
    - Only first block followed by pooling
    - Time-distributed conversion: (B, C, H, W) -> sequence length = W, features = C * H
    - GRU -> flatten -> FC(3000) -> FC(1000)
    NOTE: we adapt some filters to reasonable dims to avoid insane memory blowups.
    """
    def __init__(self, input_channels: int, output_dim: int, leaky_slope=0.1, gru_hidden=512):
        super().__init__()
        # We'll implement a deep stack with the spirit of the paper.
        # Filters chosen to follow the paper ratios but keepability in typical hardware.
        f = [64, 128, 128, 256, 256, 128, 128, 128, 128, 128, 128, 256]  # 12 entries (close to described)
        self.blocks = nn.ModuleList()
        in_ch = input_channels
        for out_ch in f:
            self.blocks.append(conv_block(in_ch, out_ch, kernel=3, stride=1, padding=1, leaky_slope=leaky_slope))
            in_ch = out_ch
        # pooling after first block only (as described)
        self.pool = nn.MaxPool2d(kernel_size=2, stride=2)
        # We'll include 3 residual shortcuts: between early, mid, late blocks
        # For simplicity implement as add of tensors with matching channel size; if mismatched, use a 1x1 conv.
        self.shortcuts = nn.ModuleDict()
        # define shortcuts pairs (from idx -> to idx)
        shortcut_pairs = [(0,4), (3,8), (6,11)]
        for a,b in shortcut_pairs:
            ca = f[a]
            cb = f[b]
            if ca != cb:
                self.shortcuts[f"{a}_{b}"] = nn.Conv2d(ca, cb, kernel_size=1, stride=1, bias=False)
            else:
                self.shortcuts[f"{a}_{b}"] = nn.Identity()
        # after conv stack adaptive pooling? we'll keep H and W small using input size 100 and one pool
        # GRU expects sequences: we will take width dimension as time steps
        # We'll not reduce further; but to avoid huge dims, add one more pooling near the end
        self.pool_end = nn.AdaptiveAvgPool2d((8, 8))  # reduce to 8x8 feature map
        # GRU: input_size = channels * 8  (sequence length = 8)
        final_channels = f[-1]
        gru_input_size = final_channels * 8
        self.gru = nn.GRU(input_size=gru_input_size, hidden_size=gru_hidden, batch_first=True)
        # FC layers
        self.fc1 = nn.Linear(gru_hidden * 8, 3000)  # flatten GRU outputs across time
        self.act = nn.LeakyReLU(negative_slope=leaky_slope, inplace=True)
        self.fc2 = nn.Linear(3000, 1000)  # article uses 1000 outputs; but we will adapt if output_dim !=1000 later
        # if output_dim differs, final projection will be adjusted in wrapper

    def forward(self, x):
        # x: (B, C, H, W)
        out = x
        intermediates = {}
        for i, blk in enumerate(self.blocks):
            out = blk(out)
            if i == 0:
                out = self.pool(out)
            intermediates[i] = out
            # apply shortcuts if present as destination
            # we'll add after constructing full forward (done below)
        # apply explicit shortcuts additions (simpler to re-evaluate)
        # reconstruct forward with additions to match intended skip connections:
        out = x
        out = self.blocks[0](out)
        out = self.pool(out)  # after first block
        prevs = {0: out}
        for i in range(1, len(self.blocks)):
            cur = self.blocks[i](prevs[i-1] if (i-1) in prevs else out)
            # check incoming shortcuts targeting i (we defined 0->4, 3->8, 6->11)
            # if i is a destination, add the source mapped through conv if needed
            if f"{0}_{4}" == f"{i-4}_{i}" if False else False:
                pass  # keep structure simple: below we'll just add if indices match pairs
            prevs[i] = cur
        # Simpler and deterministic approach: sequential apply and add predefined shortcuts when arriving at dest indices:
        out = x
        out = self.blocks[0](out)
        out = self.pool(out)
        saved = {}
        saved[0] = out.clone()
        for i in range(1, len(self.blocks)):
            out = self.blocks[i](out)
            # if this index is a destination of a shortcut add:
            for key in self.shortcuts:
                a,b = map(int, key.split('_'))
                if i == b:
                    src = saved.get(a, None)
                    if src is not None:
                        transform = self.shortcuts[key]
                        out = out + transform(src)
            saved[i] = out.clone()
        # final adaptive pooling to fixed spatial dims
        out = self.pool_end(out)  # (B, C, 8, 8)
        B, C, H, W = out.shape
        # prepare sequence: use width as time dimension -> seq_len = W (8), features = C * H
        # permute to (B, seq_len, features)
        seq = out.permute(0, 3, 1, 2).contiguous()  # (B, W, C, H)
        seq = seq.view(B, W, C * H)                 # (B, seq_len, feat)
        # GRU
        gru_out, _ = self.gru(seq)  # (B, seq_len, hidden)
        # flatten across time
        flat = gru_out.contiguous().view(B, -1)  # (B, seq_len * hidden)
        x = self.act(self.fc1(flat))
        x = self.fc2(x)  # (B, 1000)
        return x

class CNN_GRU_Reduced(nn.Module):
    """
    Reduced architecture: fewer conv blocks + GRU + smaller FCs suitable for small datasets.
    """
    def __init__(self, input_channels: int, output_dim: int, leaky_slope=0.1, gru_hidden=256):
        super().__init__()
        self.conv1 = conv_block(input_channels, 32, kernel=3, padding=1, leaky_slope=leaky_slope)
        self.conv2 = conv_block(32, 64, kernel=3, padding=1, leaky_slope=leaky_slope)
        self.conv3 = conv_block(64, 128, kernel=3, padding=1, leaky_slope=leaky_slope)
        self.pool = nn.MaxPool2d(2,2)
        self.pool_end = nn.AdaptiveAvgPool2d((8,8))
        final_ch = 128
        self.gru = nn.GRU(input_size=final_ch*8, hidden_size=gru_hidden, batch_first=True)
        self.fc1 = nn.Linear(gru_hidden*8, 512)
        self.act = nn.LeakyReLU(negative_slope=leaky_slope, inplace=True)
        self.fc2 = nn.Linear(512, output_dim)

    def forward(self,x):
        x = self.conv1(x)
        x = self.pool(x)
        x = self.conv2(x)
        x = self.conv3(x)
        x = self.pool_end(x)
        B,C,H,W = x.shape
        seq = x.permute(0,3,1,2).contiguous().view(B, W, C*H)
        gru_out, _ = self.gru(seq)
        flat = gru_out.contiguous().view(B, -1)
        x = self.act(self.fc1(flat))
        x = self.fc2(x)
        return x

# wrapper that adapts final output size if needed
class ModelWrapper(nn.Module):
    def __init__(self, base_model: nn.Module, final_output_dim: int):
        super().__init__()
        self.base = base_model
        # base may output 1000 for original; if desired output differs, apply final projection
        self.final_output_dim = final_output_dim
        # inspect a sample forward? We'll always include a linear projection if needed.
        self.proj = None
        # create a linear layer lazily when first forward runs if output mismatch occurs
        self._lazy_proj = True

    def forward(self, x):
        out = self.base(x)
        if self._lazy_proj:
            if out.shape[1] != self.final_output_dim:
                self.proj = nn.Linear(out.shape[1], self.final_output_dim).to(out.device)
            else:
                self.proj = None
            self._lazy_proj = False
        if self.proj is not None:
            out = self.proj(out)
        return out

# -----------------------
# Training utilities
# -----------------------
def train_model(model, dataloaders, criterion, optimizer, device,
                num_epochs=500, save_dir='results', patience=None, scaler=None):
    since = time.time()
    best_model_wts = copy.deepcopy(model.state_dict())
    best_val_loss = float('inf')
    history = {'train_loss': [], 'val_loss': []}
    os.makedirs(save_dir, exist_ok=True)

    for epoch in range(num_epochs):
        # train
        model.train()
        running_loss = 0.0
        n = 0
        for x,y in dataloaders['train']:
            x = x.to(device)
            y = y.to(device)
            optimizer.zero_grad()
            out = model(x)
            loss = criterion(out, y)
            loss.backward()
            optimizer.step()
            running_loss += loss.item() * x.size(0)
            n += x.size(0)
        train_loss = running_loss / n
        history['train_loss'].append(train_loss)

        # val
        model.eval()
        running_loss = 0.0
        n = 0
        with torch.no_grad():
            for x,y in dataloaders['val']:
                x = x.to(device)
                y = y.to(device)
                out = model(x)
                loss = criterion(out, y)
                running_loss += loss.item() * x.size(0)
                n += x.size(0)
        val_loss = running_loss / max(1,n)
        history['val_loss'].append(val_loss)

        # Save checkpoints based on validation loss to avoid test leakage.
        if val_loss < best_val_loss:
            best_val_loss = val_loss
            best_model_wts = copy.deepcopy(model.state_dict())
            best_ckpt = {'model_state': best_model_wts}
            if scaler is not None:
                best_ckpt['scaler_mean'] = scaler.mean_.tolist()
                best_ckpt['scaler_scale'] = scaler.scale_.tolist()
            torch.save(best_ckpt, os.path.join(save_dir, 'best_by_val.pth'))
            # Compatibility alias used by existing scripts.
            torch.save(best_ckpt, os.path.join(save_dir, 'best_by_test.pth'))

        if epoch % 10 == 0 or epoch==0:
            print(f"Epoch {epoch+1}/{num_epochs} | train {train_loss:.4e} | val {val_loss:.4e}")

        # optional early stopping by val if patience provided
        # (but paper ran full epochs and selected best on test)
        if patience is not None:
            # implement patience on val
            if len(history['val_loss'])>patience:
                recent = history['val_loss'][-(patience+1):-1]
                if min(recent) <= val_loss:
                    print("Early stopping triggered by val patience.")
                    break

    time_elapsed = time.time() - since
    print(f"Training complete in {time_elapsed//60:.0f}m {time_elapsed%60:.0f}s. Best val loss: {best_val_loss:.6e}")
    model.load_state_dict(best_model_wts)
    return model, history

def evaluate_model(model, dataloader, device):
    model.eval()
    preds = []
    trues = []
    with torch.no_grad():
        for x,y in dataloader:
            x = x.to(device)
            out = model(x)
            preds.append(out.cpu().numpy())
            trues.append(y.numpy())
    preds = np.concatenate(preds, axis=0)
    trues = np.concatenate(trues, axis=0)
    mse = np.mean((preds - trues)**2)
    return mse, preds, trues

# -----------------------
# plotting / prediction
# -----------------------
def smooth(y, window=9):
    try:
        from scipy.signal import savgol_filter
        wl = min(window, len(y)-1 if (len(y)-1)%2==1 else len(y))
        if wl %2 == 0: wl -= 1
        if wl < 3:
            return y
        return savgol_filter(y, wl, 3)
    except Exception:
        # simple smoothing fallback
        return np.convolve(y, np.ones(5)/5.0, mode='same')

def inverse_transform(y, scaler):
    if scaler is None:
        return y
    return scaler.inverse_transform(y.reshape(1,-1)).reshape(-1)

def predict_and_plot(model, img_path, csv_path, scaler, device, keep_rgb=False, img_size=100, savepath=None, show_plot=True):
    lambdas, y_true = read_spectrum_csv(csv_path)
    img = Image.open(img_path).convert('RGB')
    # transform consistent with dataset
    if keep_rgb:
        t = T.Compose([T.Resize((img_size,img_size)), T.ToTensor()])
    else:
        t = T.Compose([T.Resize((img_size,img_size)), T.Grayscale(num_output_channels=1), T.ToTensor()])
    x = t(img).unsqueeze(0).to(device)
    model.eval()
    with torch.no_grad():
        y_pred = model(x).cpu().numpy().reshape(-1)
    y_pred_inv = inverse_transform(y_pred, scaler)
    y_true_inv = y_true
    y_pred_inv = smooth(y_pred_inv)
    # plot
    fig, axs = plt.subplots(1,2, figsize=(10,4))
    axs[0].imshow(img)
    axs[0].axis('off')
    axs[0].set_title(os.path.basename(img_path))
    axs[1].plot(lambdas, y_true_inv, label='True')
    axs[1].plot(lambdas, y_pred_inv, label='Predicted')
    axs[1].legend()
    axs[1].set_xlabel('lambda')
    if savepath:
        plt.savefig(savepath, dpi=150)
    if show_plot:
        plt.show()
    plt.close(fig)

# -----------------------
# Main
# -----------------------
def main(args):
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    print("Device:", device)
    set_global_seed(args.seed)
    print("Seed:", args.seed)

    pairs = find_pairs(args.data_dir)
    if len(pairs) == 0:
        raise RuntimeError("No image/csv pairs found. Check data_dir and filenames.")
    print(f"Found {len(pairs)} pairs.")
    if args.max_samples is not None:
        pairs = pairs[:args.max_samples]

    train_dataset = SpectrumImageDataset(
        pairs,
        img_size=args.img_size,
        keep_rgb=args.keep_rgb,
        augment=True,
        normalize_targets=args.normalize_targets,
    )
    eval_dataset = SpectrumImageDataset(
        pairs,
        img_size=args.img_size,
        keep_rgb=args.keep_rgb,
        augment=False,
        normalize_targets=args.normalize_targets,
    )

    n = len(pairs)
    n_train = max(1, int(0.8 * n))
    n_test = max(1, int(0.1 * n))
    n_val = n - n_train - n_test
    if n_val < 0:
        n_val = 0
        n_test = n - n_train

    split_generator = torch.Generator().manual_seed(args.seed)
    permutation = torch.randperm(n, generator=split_generator).tolist()
    train_indices = permutation[:n_train]
    test_indices = permutation[n_train:n_train + n_test]
    val_indices = permutation[n_train + n_test:n_train + n_test + n_val]

    train_index_set = set(train_indices)
    test_index_set = set(test_indices)
    val_index_set = set(val_indices)
    if train_index_set & test_index_set or train_index_set & val_index_set or test_index_set & val_index_set:
        raise RuntimeError("Split overlap detected between train/val/test.")

    if args.normalize_targets:
        train_pairs = [pairs[i] for i in train_indices]
        scaler = train_dataset.fit_scaler(train_pairs)
    else:
        scaler = None
    train_dataset.scaler = scaler
    eval_dataset.scaler = scaler

    train_ds = Subset(train_dataset, train_indices)
    val_ds = Subset(eval_dataset, val_indices)
    test_ds = Subset(eval_dataset, test_indices)

    save_split_manifest(
        save_dir=args.save_dir,
        data_dir=args.data_dir,
        seed=args.seed,
        pairs=pairs,
        train_indices=train_indices,
        val_indices=val_indices,
        test_indices=test_indices,
    )

    train_loader = DataLoader(train_ds, batch_size=args.batch_size, shuffle=True, num_workers=0)
    val_loader = DataLoader(val_ds, batch_size=args.batch_size, shuffle=False, num_workers=0)
    test_loader = DataLoader(test_ds, batch_size=args.batch_size, shuffle=False, num_workers=0)
    dataloaders = {'train': train_loader, 'val': val_loader}

    output_dim = train_dataset.spectrum_len
    print("Spectrum length (output_dim):", output_dim)

    # choose model
    in_ch = 3 if args.keep_rgb else 1
    if args.model == 'original':
        base = CNN_GRU_Original(input_channels=in_ch, output_dim=output_dim)
    else:
        base = CNN_GRU_Reduced(input_channels=in_ch, output_dim=output_dim)
    model = ModelWrapper(base, final_output_dim=output_dim).to(device)

    criterion = nn.MSELoss()
    # Nadam optimizer (Nesterov Adam approx from paper): use torch.optim.NAdam
    optimizer = torch.optim.NAdam(model.parameters(), lr=args.lr)

    # train
    model, history = train_model(model, dataloaders, criterion, optimizer, device,
                                num_epochs=args.epochs, save_dir=args.save_dir,
                                patience=args.patience, scaler=scaler)

    # evaluate final on test
    test_mse, preds, trues = evaluate_model(model, test_loader, device)
    print("Final test MSE:", test_mse)

    # Save numerical results (predictions vs true values)
    np.savez(os.path.join(args.save_dir, 'test_results.npz'), preds=preds, trues=trues, test_mse=test_mse)

    # Save training history plot
    plt.figure(figsize=(10, 6))
    plt.plot(history['train_loss'], label='Train Loss')
    plt.plot(history['val_loss'], label='Val Loss')

    plt.xlabel('Epoch')
    plt.ylabel('MSE Loss')
    plt.title('Training Convergence')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.savefig(os.path.join(args.save_dir, 'loss_history.png'))
    plt.close()

    # save final model and scaler
    os.makedirs(args.save_dir, exist_ok=True)
    ckpt = {'model_state': model.state_dict()}
    if scaler is not None:
        ckpt['scaler_mean'] = scaler.mean_.tolist()
        ckpt['scaler_scale'] = scaler.scale_.tolist()
    torch.save(ckpt, os.path.join(args.save_dir, 'final_checkpoint.pth'))
    print("Saved final checkpoint to", os.path.join(args.save_dir, 'final_checkpoint.pth'))

    # Save multiple prediction plots from the test set
    print("Generating prediction plots for test samples...")
    # Plot up to 10 random samples from test set (or first 10)
    num_plots = min(10, len(test_indices))
    
    for i in range(num_plots):
        idx = test_indices[i]
        img_path, csv_path = pairs[idx]
        save_name = os.path.join(args.save_dir, f'test_prediction_{i+1}.png')
        predict_and_plot(model, img_path, csv_path, scaler, device, keep_rgb=args.keep_rgb,
                         img_size=args.img_size, savepath=save_name, show_plot=False)
    
    print(f"Saved {num_plots} test prediction plots to {args.save_dir}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--data_dir', type=str, default='../Dados - NanoComp/Graficos')
    parser.add_argument('--save_dir', type=str, default='Article_Results')
    parser.add_argument('--model', type=str, choices=['original','reduced'], default='reduced',
                        help='Choose "original" (paper) or "reduced" (smaller for few data)')
    parser.add_argument('--img_size', type=int, default=100)
    parser.add_argument('--keep_rgb', action='store_true', help='Do not convert to grayscale')
    parser.add_argument('--normalize_targets', action='store_true', help='standardize targets (recommended)')
    parser.add_argument('--batch_size', type=int, default=8)
    parser.add_argument('--epochs', type=int, default=500)
    parser.add_argument('--lr', type=float, default=1e-4)
    parser.add_argument('--patience', type=int, default=None)
    parser.add_argument('--max_samples', type=int, default=None)
    parser.add_argument('--seed', type=int, default=33,
                        help='random seed for reproducible split and training')
    args = parser.parse_args()
    main(args)