import sys
from PIL import Image
import os

def convert_png_to_rgb_binary(image_path):
    try:
        if not image_path.lower().endswith(".png"):
            raise ValueError("Input file must have a .png extension.")
        output_binary_path = os.path.splitext(image_path)[0] + ".bin"
        img = Image.open(image_path)
        img = img.convert("RGB")
        pixels = list(img.getdata())
        width, height = img.size

        with open(output_binary_path, 'wb') as f:
            f.write(width.to_bytes(4, byteorder='big'))
            f.write(height.to_bytes(4, byteorder='big')) 
            for rgb in pixels:
                f.write(bytes(rgb))

        print(f"RGB values and dimensions saved to {output_binary_path}")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <input_png_path>")
        sys.exit(1)

    image_path = sys.argv[1]
    convert_png_to_rgb_binary(image_path)
