from PIL import Image

def convert_png_to_rgb_binary(image_path, output_binary_path):
    try:
        # Open the image
        img = Image.open(image_path)

        # Ensure the image is in RGB mode
        img = img.convert("RGB")

        # Get the pixel data
        pixels = list(img.getdata())

        # Get image dimensions
        width, height = img.size

        # Write dimensions and RGB pairs to a binary file
        with open(output_binary_path, 'wb') as f:
            f.write(width.to_bytes(4, byteorder='big'))  # Write width as 4 bytes
            f.write(height.to_bytes(4, byteorder='big'))  # Write height as 4 bytes
            for rgb in pixels:
                f.write(bytes(rgb))

        print(f"RGB values and dimensions saved to {output_binary_path}")
    except Exception as e:
        print(f"Error: {e}")

# Example usage
image_path = "img_7660.png"  # Path to the PNG image
output_binary_path = "img_7660.bin"  # Path to the output binary file
convert_png_to_rgb_binary(image_path, output_binary_path)