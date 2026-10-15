#!/usr/bin/env python3
"""
Process COMSOL simulation files (Imput/Output txt) to generate:
- CSV files (one per angle) with simulation data
- PNG images (100x100) showing the nanoparticle shape at each angle
"""

import os
import csv
import math
from PIL import Image, ImageDraw


SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_DIR = os.path.dirname(SCRIPT_DIR)
SIM_INPUT_DIR = os.path.join(PROJECT_DIR, "Simulation_Input")


def parse_input(filepath):
    """Parse Imput_XX.txt -> list of (x, y) coordinates."""
    coords = []
    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            if line:
                parts = line.split()
                coords.append((float(parts[0]), float(parts[1])))
    return coords


def parse_output(filepath):
    """Parse Output_XX.txt -> dict {angle: [(lambda, field), ...]}."""
    data = {}
    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('%'):
                continue
            parts = line.split()
            angle = float(parts[0])
            lam = float(parts[1])
            field = float(parts[2])
            data.setdefault(angle, []).append((lam, field))
    return data


def save_csvs(data, sim_num, output_dir):
    """Save one CSV per angle. Returns sorted list of angles."""
    angles = sorted(data.keys())
    for i, angle in enumerate(angles, 1):
        path = os.path.join(output_dir, f"Grafico_{sim_num:02d}_{i}.csv")
        with open(path, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['angle', 'lambda', 'electric_field_norm'])
            for lam, field in data[angle]:
                writer.writerow([angle, lam, field])
    return angles


def generate_images(coords, angles, sim_num, output_dir):
    """Generate 100x100 white-background PNG images with black polygon outline.

    The base shape is drawn once, then rotated by each angle. Scaling uses
    the max radius from center so rotation never clips the shape.
    """
    size = 100
    padding = 5

    # Center of polygon bounding box
    xs = [c[0] for c in coords]
    ys = [c[1] for c in coords]
    cx = (min(xs) + max(xs)) / 2
    cy = (min(ys) + max(ys)) / 2

    # Scale by max radius so the shape fits a circle inside the image
    max_radius = max(math.sqrt((x - cx) ** 2 + (y - cy) ** 2) for x, y in coords)
    scale = ((size / 2) - padding) / max_radius

    def to_pixel(x, y):
        return ((x - cx) * scale + size / 2, -(y - cy) * scale + size / 2)

    # Draw base shape (angle 0)
    base = Image.new('RGB', (size, size), (255, 255, 255))
    draw = ImageDraw.Draw(base)
    pixels = [to_pixel(x, y) for x, y in coords]
    for j in range(len(pixels) - 1):
        draw.line([pixels[j], pixels[j + 1]], fill=(0, 0, 0), width=2)

    # Save each rotation
    for i, angle in enumerate(angles, 1):
        if angle == 0:
            img = base.copy()
        else:
            img = base.rotate(angle, resample=Image.BICUBIC,
                              center=(size / 2, size / 2),
                              fillcolor=(255, 255, 255))
        img.save(os.path.join(output_dir, f"Imagem_{sim_num:02d}_{i}.png"))


def process_simulation(sim_num):
    """Process one Imput/Output pair."""
    input_path = os.path.join(SIM_INPUT_DIR, f"Imput_{sim_num:02d}.txt")
    output_path = os.path.join(SIM_INPUT_DIR, f"Output_{sim_num:02d}.txt")

    if not os.path.exists(input_path):
        print(f"  Skipping {sim_num:02d}: missing {os.path.basename(input_path)}")
        return
    if not os.path.exists(output_path):
        print(f"  Skipping {sim_num:02d}: missing {os.path.basename(output_path)}")
        return

    coords = parse_input(input_path)
    # Close polygon if first and last points differ
    if coords and coords[0] != coords[-1]:
        coords.append(coords[0])
    data = parse_output(output_path)
    angles = save_csvs(data, sim_num, SIM_INPUT_DIR)
    generate_images(coords, angles, sim_num, SIM_INPUT_DIR)
    print(f"  Simulation {sim_num:02d}: {len(angles)} CSVs + {len(angles)} images generated")


def main():
    if not os.path.exists(SIM_INPUT_DIR):
        print(f"Directory not found: {SIM_INPUT_DIR}")
        return

    sim_nums = sorted(
        int(f.replace("Imput_", "").replace(".txt", ""))
        for f in os.listdir(SIM_INPUT_DIR)
        if f.startswith("Imput_") and f.endswith(".txt")
    )

    if not sim_nums:
        print("No Imput_XX.txt files found in Simulation_Input/")
        return

    print(f"Found {len(sim_nums)} simulation(s)")
    for num in sim_nums:
        process_simulation(num)
    print("Done!")


if __name__ == "__main__":
    main()
