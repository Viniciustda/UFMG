"""
TestModel.py

Description: Evaluation and Testing Script.
             Loads a trained model and compares the AI prediction against a
             real simulation (CSV file) using the same plotting logic from
             NeuralNetwork.py.
             
             Modes:
             1. Automatic: Provide --data_dir. The script picks a random pair.
             2. Manual: Provide --img and --csv paths directly.

Save in: Plasmonica/Code/TestModel.py

Usage (Automatic - Random Sample):
    python Code/TestModel.py \
        --checkpoint "Article_Results_Training/best_by_test.pth" \
        --data_dir "Data - NanoComp/Organized_Dataset" \
        --model reduced \
        --img_size 100

Usage (Manual - Specific File):
    python Code/TestModel.py \
        --checkpoint "Article_Results_Training/best_by_test.pth" \
        --img "Article_Results_Test/Imagem_12_6.png" \
        --csv "Article_Results_Test/Grafico_12_6.csv" \
        --model reduced \
        --img_size 100

Usage (Batch - Save All Plots):
    python Code/TestModel.py \
        --checkpoint "Article_Results_Training/final_checkpoint.pth" \
        --data_dir "Simulation_Input" \
        --all_pairs \
        --output_dir "Simulation_Output" \
        --no_show
"""

import torch
import argparse
import random
import os
import sys
import numpy as np
from sklearn.preprocessing import StandardScaler

from NeuralNetwork import (
    ModelWrapper,
    CNN_GRU_Reduced,
    CNN_GRU_Original,
    find_pairs,
    read_spectrum_csv,
    predict_and_plot,
)

def load_scaler_from_checkpoint(ckpt):
    if 'scaler_mean' in ckpt and 'scaler_scale' in ckpt:
        scaler = StandardScaler()
        scaler.mean_ = np.array(ckpt['scaler_mean'])
        scaler.scale_ = np.array(ckpt['scaler_scale'])
        scaler.var_ = scaler.scale_ ** 2
        return scaler
    return None

def main():
    parser = argparse.ArgumentParser(description="Test script using the same visualization as NeuralNetwork.py")
    parser.add_argument('--checkpoint', type=str, required=True, help='Path to .pth checkpoint')
    parser.add_argument('--model', type=str, choices=['original', 'reduced'], default='reduced', help='Model architecture')
    parser.add_argument('--img_size', type=int, default=100, help='Image resolution')
    parser.add_argument('--keep_rgb', action='store_true', help='Keep RGB channels')
    parser.add_argument('--all_pairs', action='store_true',
                        help='With --data_dir, process all pairs instead of selecting one at random')
    parser.add_argument('--output_dir', type=str, default=None,
                        help='Directory to save comparison plots')
    parser.add_argument('--no_show', action='store_true',
                        help='Do not open matplotlib windows (recommended for batch mode)')
    
    # Mutually exclusive group for file selection
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--data_dir', type=str, help='Directory for automatic random selection')
    group.add_argument('--img', type=str, help='Specific image path for manual selection')

    parser.add_argument('--csv', type=str, help='Specific CSV path (required for manual mode)')

    args = parser.parse_args()

    # File Selection Logic
    if args.data_dir:
        print(f"Searching for pairs in: {args.data_dir}...")
        pairs = find_pairs(args.data_dir)
        if not pairs:
            print("Error: No pairs found in the directory.")
            sys.exit(1)
        if args.all_pairs:
            selected_pairs = pairs
            print(f"Found {len(pairs)} pairs. Processing all pairs...")
        else:
            print(f"Found {len(pairs)} pairs. Selecting one at random...")
            selected_pairs = [random.choice(pairs)]
    else:
        if not args.csv:
            print("Error: Manual mode requires --csv")
            sys.exit(1)
        selected_pairs = [(args.img, args.csv)]

    if args.all_pairs and not args.no_show:
        print("Info: --all_pairs enabled. Forcing --no_show to avoid opening many windows.")
        args.no_show = True

    if args.output_dir:
        os.makedirs(args.output_dir, exist_ok=True)

    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

    if not os.path.exists(args.checkpoint):
        print(f"Checkpoint not found: {args.checkpoint}")
        sys.exit(1)

    ckpt = torch.load(args.checkpoint, map_location=device)
    
    first_img, first_csv = selected_pairs[0]
    _, y_temp = read_spectrum_csv(first_csv)
    output_dim = len(y_temp)

    in_ch = 3 if args.keep_rgb else 1
    if args.model == 'original':
        base = CNN_GRU_Original(in_ch, output_dim)
    else:
        base = CNN_GRU_Reduced(in_ch, output_dim)

    model = ModelWrapper(base, final_output_dim=output_dim).to(device)
    # Dummy forward pass to initialize lazy layers
    _ = model(torch.zeros(1, in_ch, args.img_size, args.img_size).to(device))
    
    try:
        model.load_state_dict(ckpt['model_state'])
    except RuntimeError as e:
        print(f"Error loading weights: {e}")
        print("Check if --model matches the training configuration.")
        sys.exit(1)

    scaler = load_scaler_from_checkpoint(ckpt)
    if scaler is None:
        print("Warning: checkpoint does not contain scaler. For exact NeuralNetwork-style values, use final_checkpoint.pth.")

    total = len(selected_pairs)
    for i, (img_path, csv_path) in enumerate(selected_pairs, start=1):
        print(f"[{i}/{total}] Processing {os.path.basename(img_path)}")
        save_path = None
        if args.output_dir:
            base = os.path.splitext(os.path.basename(img_path))[0]
            save_path = os.path.join(args.output_dir, f"{base}_comparison.png")

        predict_and_plot(
            model=model,
            img_path=img_path,
            csv_path=csv_path,
            scaler=scaler,
            device=device,
            keep_rgb=args.keep_rgb,
            img_size=args.img_size,
            savepath=save_path,
            show_plot=not args.no_show,
        )

        if save_path:
            print(f"Saved plot to: {save_path}")

    if args.output_dir:
        print(f"Done. Saved {total} plot(s) to: {args.output_dir}")

if __name__ == "__main__":
    main()