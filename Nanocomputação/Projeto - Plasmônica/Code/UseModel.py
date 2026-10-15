"""
UseModel.py

Description: Inference Script (Real-world Use).
             Uses the neural network to predict the spectrum of a new structure
             based solely on the image, without needing a CSV file (ground truth).

Save in: Plasmonica/Code/UseModel.py

Usage (example):
    cd Plasmonica
    python Code/UseModel.py \
        --checkpoint "Article_Results/best_by_test.pth" \
        --img "Data - NanoComp/Test/Image_50.png" \
        --model reduced \
        --points 1000 

Notes:
 - Does not require a CSV. Ideal for predicting results for hand-drawn or new structures.
 - It is NECESSARY to provide --points (e.g., 1000) corresponding to the training output size,
   as there is no CSV for the code to count lines automatically.
 - The X-axis (Wavelength) is estimated (e.g., 800nm to 1700nm) for visualization purposes only,
   as this physical information existed only in the original CSV.
"""

import torch
import argparse
import os
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image
import torchvision.transforms as T

from NeuralNetwork import ModelWrapper, CNN_GRU_Reduced, CNN_GRU_Original

def load_scaler_from_checkpoint(ckpt):
    """Retrieves the normalization 'ruler' saved during training."""
    if 'scaler_mean' in ckpt and 'scaler_scale' in ckpt:
        # We need to manually reconstruct the sklearn scaler object
        from sklearn.preprocessing import StandardScaler
        scaler = StandardScaler()
        scaler.mean_ = np.array(ckpt['scaler_mean'])
        scaler.scale_ = np.array(ckpt['scaler_scale'])
        scaler.var_ = scaler.scale_ ** 2  # variance is scale squared
        return scaler
    return None

def main():
    parser = argparse.ArgumentParser(description="Script to use the neural network in practice (Inference)")
    parser.add_argument('--checkpoint', type=str, required=True, help='Path to the trained .pth file')
    parser.add_argument('--img', type=str, required=True, help='Image of the structure to predict')
    parser.add_argument('--model', type=str, choices=['original', 'reduced'], default='reduced', help='Which model was used?')
    parser.add_argument('--points', type=int, default=1000, help='How many points does the spectrum have? (Ex: 1000, 300...)')
    parser.add_argument('--keep_rgb', action='store_true', help='Keep RGB (if trained that way)')
    parser.add_argument('--save_plot', type=str, default='Forecast.png', help='Filename to save the plot')
    
    args = parser.parse_args()
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

    print(f"--- Starting Inference ---")
    print(f"Model: {args.model}")
    print(f"Image: {args.img}")

    # 1. Prepare the Image
    img_size = 100 # Fixed according to your training
    img = Image.open(args.img).convert('RGB')
    
    if args.keep_rgb:
        transform = T.Compose([T.Resize((img_size, img_size)), T.ToTensor()])
        in_ch = 3
    else:
        # Convert to Grayscale if it is the default (B/W)
        transform = T.Compose([
            T.Resize((img_size, img_size)), 
            T.Grayscale(num_output_channels=1), 
            T.ToTensor()
        ])
        in_ch = 1
        
    img_tensor = transform(img).unsqueeze(0).to(device) # Add batch dimension (1, C, H, W)

    # 2. Build the "Empty" Network
    output_dim = args.points # We need to provide this manually now!
    
    if args.model == 'original':
        base = CNN_GRU_Original(input_channels=in_ch, output_dim=output_dim)
    else:
        base = CNN_GRU_Reduced(input_channels=in_ch, output_dim=output_dim)
        
    model = ModelWrapper(base, final_output_dim=output_dim).to(device)

    # Initialize linear layer if necessary (Dummy forward)
    dummy = torch.zeros(1, in_ch, img_size, img_size).to(device)
    _ = model(dummy)

    # 3. Load the "Brain" (Weights + Scaler)
    if not os.path.exists(args.checkpoint):
        raise FileNotFoundError(f"Checkpoint not found: {args.checkpoint}")
        
    ckpt = torch.load(args.checkpoint, map_location=device)
    model.load_state_dict(ckpt['model_state'])
    scaler = load_scaler_from_checkpoint(ckpt)
    
    model.eval()
    print("Model loaded successfully.")

    # 4. Prediction
    with torch.no_grad():
        prediction = model(img_tensor).cpu().numpy().reshape(-1)

    # 5. Undo Normalization (Revert to real values)
    if scaler:
        # The scaler expects format (N_samples, N_features)
        prediction_real = scaler.inverse_transform(prediction.reshape(1, -1)).reshape(-1)
    else:
        prediction_real = prediction

    # 6. Plot the Result
    # Since we don't have the CSV, we don't know the exact wavelengths (e.g., 800nm to 1700nm).
    # We will create a generic X-axis or based on the article's range.
    # The article uses a range of ~800 to 1700 nm with 1000 points.
    x_axis = np.linspace(800, 1700, len(prediction_real)) 

    plt.figure(figsize=(10, 5))
    
    # Subplot 1: The Image
    plt.subplot(1, 2, 1)
    plt.imshow(img)
    plt.title("Structure (Input)")
    plt.axis('off')

    # Subplot 2: The Predicted Plot
    plt.subplot(1, 2, 2)
    plt.plot(x_axis, prediction_real, label='AI Prediction', color='orange', linewidth=2)
    plt.title("Estimated Absorption Spectrum")
    plt.xlabel("Wavelength (nm) [Estimated]")
    plt.ylabel("Absorption / Electric Field")
    plt.grid(True, alpha=0.3)
    plt.legend()

    plt.tight_layout()
    plt.savefig(args.save_plot)
    print(f"Plot saved to: {args.save_plot}")
    plt.show()

if __name__ == "__main__":
    main()