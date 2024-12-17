import math
from utils import sphere_angle
import time
from matplotlib import pyplot as plt
import random
from collections import defaultdict


def _parse_catalog(file_path):
    stars = []

    with open(file_path, 'r') as file:
        for line in file:
            parts = line.strip().split('|')

            # Parse individual fields
            raj2000 = float(parts[0])
            dej2000 = float(parts[1])
            name = int(parts[2])

            magnitude = float(parts[4])

            star = (
                math.radians(raj2000),
                math.radians(dej2000),
                magnitude,
                name
            )

            stars.append(star)

    return stars


def _remove_close_stars(catalog, threshold_degrees=0.2, num_brightest=1000):
    sorted_catalog = sorted(catalog, key=lambda star: star[2])

    brightest_stars = sorted_catalog[:num_brightest]

    threshold_radians = math.radians(threshold_degrees)
    cos_threshold = math.cos(threshold_radians)

    to_remove = set()

    for i in range(len(brightest_stars)):
        if i in to_remove:
            continue
        for j in range(i + 1, len(brightest_stars)):
            if j in to_remove:
                continue

            cos_distance = sphere_angle(brightest_stars[i], brightest_stars[j])

            if cos_distance > cos_threshold:
                to_remove.add(i)
                to_remove.add(j)

    new_catalog = [star for index, star in enumerate(brightest_stars) if index not in to_remove]

    return new_catalog


def _generate_map(catalog, threshold_degrees=15):
    threshold_cosine = math.cos(math.radians(threshold_degrees))
    
    stars_a = []
    stars_b = []
    cossphere_angle_list = []

    for i in range(len(catalog)):
        for j in range(i + 1, len(catalog)):
            star1 = catalog[i]
            star2 = catalog[j]
            
            cos_distance = sphere_angle(star1, star2)
            
            if cos_distance >= threshold_cosine:
                stars_a.append(star1[3]) 
                stars_b.append(star2[3])  
                cossphere_angle_list.append(cos_distance)

    # Sort the lists based on the cosine of the angle in increasing order
    sorted_indexes = sorted(range(len(cossphere_angle_list)), key=lambda i: cossphere_angle_list[i])

    # Apply sorting to all three lists
    stars_a = [stars_a[i] for i in sorted_indexes]
    stars_b = [stars_b[i] for i in sorted_indexes]
    cossphere_angle_list = [cossphere_angle_list[i] for i in sorted_indexes]

    return stars_a, stars_b, cossphere_angle_list


def run_database(file_path):
    catalog = _parse_catalog(file_path)
    catalog = _remove_close_stars(catalog, 0.2, 2000)
    stars_a, stars_b, angles = _generate_map(catalog, 15)  
    
    print(len(angles))
    
    return stars_a, stars_b, angles, catalog


if __name__ == "__main__":
    file_path = "bright-star-catalog.tsv"
    catalog = _parse_catalog(file_path)
    catalog = _remove_close_stars(catalog, 0.2, 1000)
    stars_a, stars_b, angles = _generate_map(catalog, 15)
    
    
    
                