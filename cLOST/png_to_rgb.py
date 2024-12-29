import os
import sys
from PIL import Image

def convert_png_to_rgb_binary(image_path, output_binary_path):
    try:
        # Check if the output file already exists
        if os.path.exists(output_binary_path):
            # print(f"Skipping {image_path}: {output_binary_path} already exists")
            return

        # Open the image
        img = Image.open(image_path)

        # Ensure the image is in RGB mode
        img = img.convert("RGB")

        # Get the pixel data
        pixels = list(img.getdata())

        # Get image dimensions
        width, height = img.size
        print(f"Processing {image_path}: {width}x{height}")

        # Write dimensions and RGB pairs to a binary file
        with open(output_binary_path, 'wb') as f:
            f.write(width.to_bytes(4, byteorder='big'))  # Write width as 4 bytes
            f.write(height.to_bytes(4, byteorder='big'))  # Write height as 4 bytes
            for rgb in pixels:
                f.write(bytes(rgb))

        print(f"RGB values and dimensions saved to {output_binary_path}")
    except Exception as e:
        print(f"Error processing {image_path}: {e}")

def process_files(input_path):
    if os.path.isfile(input_path):  # If it's a file, process it directly
        output_binary_path = input_path.rsplit('.', 1)[0] + '.bin'
        convert_png_to_rgb_binary(input_path, output_binary_path)
    elif os.path.isdir(input_path):  # If it's a directory, process all PNG files in it
        for root, dirs, files in os.walk(input_path):
            for file in files:
                if file.lower().endswith('.png'):
                    image_path = os.path.join(root, file)
                    output_binary_path = os.path.splitext(image_path)[0] + '.bin'
                    convert_png_to_rgb_binary(image_path, output_binary_path)

if __name__ == "__main__":
    input_path = sys.argv[1]  # First command-line argument
    process_files(input_path)
