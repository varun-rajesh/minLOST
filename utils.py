import math

def import_binary(filepath):
    with open(filepath, 'rb') as f:
        # Read dimensions
        width = int.from_bytes(f.read(4), byteorder='big')
        height = int.from_bytes(f.read(4), byteorder='big')

        # Read RGB pixel data
        pixels = []
        while rgb := f.read(3):  # Read 3 bytes at a time (RGB tuple)
            r, g, b = rgb
            # Convert to grayscale using the formula: Gray = 0.299*R + 0.587*G + 0.114*B
            gray = int(0.299 * r + 0.587 * g + 0.114 * b)
            pixels.append(gray)

    # Convert grayscale list into a 2D array
    grayscale_image = [pixels[i * width:(i + 1) * width] for i in range(height)]

    print(f"Image dimensions: {width}x{height}")
    print(f"Number of pixels: {len(pixels)}")

    return grayscale_image, height, width, len(pixels)


def sphere_angle(star1, star2):
    ra1, dec1 = star1[0], star1[1]
    ra2, dec2 = star2[0], star2[1]

    # Spherical cosine formula
    cos_distance = (
        math.sin(dec1) * math.sin(dec2) +
        math.cos(dec1) * math.cos(dec2) * math.cos(ra1 - ra2)
    )
    
    return cos_distance


def catalog_lookup(catalog, star_id):
    for entry in catalog:
        if entry[3] == star_id:
            return entry
    
    

def cat_to_spatial(vector):
    ra, dec = vector[0], vector[1]
    
    x = math.cos(dec) * math.cos(ra)
    y = math.cos(dec) * math.sin(ra)
    z = math.sin(dec)
    
    return [x, y, z]


def cam_to_spatial(vector):
    focal_length = 2207
    
    return [1, (-vector[0] + 500) / focal_length, (-vector[1] + 333) / focal_length]


def angle(v1, v2):
    v1 = cam_to_spatial(v1)
    v2 = cam_to_spatial(v2)
    
    # Dot product
    dot_prod = sum(a * b for a, b in zip(v1, v2))
    
    # Magnitudes
    mag_v1 = math.sqrt(sum(a * a for a in v1))
    mag_v2 = math.sqrt(sum(b * b for b in v2))
    
    # Calculate the cosine of the angle
    cos_theta = dot_prod / (mag_v1 * mag_v2)
    
    # Ensure the value is within the valid domain for arccos
    cos_theta = max(-1.0, min(1.0, cos_theta))
    
    # Calculate the angle in radians and convert to degrees
    # angle_deg = math.degrees(math.acos(cos_theta))
    
    return cos_theta
    
    
