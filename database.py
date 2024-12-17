import math
from utils import _sphere_angle
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

            cos_distance = _sphere_angle(brightest_stars[i], brightest_stars[j])

            if cos_distance > cos_threshold:
                to_remove.add(i)
                to_remove.add(j)

    new_catalog = [star for index, star in enumerate(brightest_stars) if index not in to_remove]

    return new_catalog


def _generate_map(catalog, threshold_degrees=15):
    threshold_cosine = math.cos(math.radians(threshold_degrees))
    
    stars_a = []
    stars_b = []
    cos_sphere_angle_list = []

    for i in range(len(catalog)):
        for j in range(i + 1, len(catalog)):
            star1 = catalog[i]
            star2 = catalog[j]
            
            cos_distance = _sphere_angle(star1, star2)
            
            if cos_distance >= threshold_cosine:
                stars_a.append(star1[3]) 
                stars_b.append(star2[3])  
                cos_sphere_angle_list.append(cos_distance)

    # Sort the lists based on the cosine of the angle in decreasing order
    sorted_indexes = sorted(range(len(cos_sphere_angle_list)), key=lambda i: cos_sphere_angle_list[i])

    # Apply sorting to all three lists
    stars_a = [stars_a[i] for i in sorted_indexes]
    stars_b = [stars_b[i] for i in sorted_indexes]
    cos_sphere_angle_list = [cos_sphere_angle_list[i] for i in sorted_indexes]

    return stars_a, stars_b, cos_sphere_angle_list


def _kvector(angles):
    # y = 0.0000018643 * x + 0.965729
    
    k = [0]
    
    for i in range(1, len(angles)):
        estimate = M * i + B
        
        for j in range(k[-1], len(angles) - 1):
            if angles[j] <= estimate <= angles[j + 1]:
                k.append(j)
                break
    
    return k


def _get_range(kvector, angle):
    bot_thresh = angle - 4 * 0.0001
    top_thresh = angle + 4 * 0.0001
    
    bot = (bot_thresh - B) / M
    top = (top_thresh - B) / M
    
    return max(0, math.floor(bot)), min(len(kvector) - 1, math.ceil(top))
    

def _validator(kvector, angles, angle):
    bot, top = _get_range(kvector, angle)
    bot, top = kvector[bot], kvector[top]
    print(top - bot)
    assert(angles[bot] < angle < angles[top]), f"{angle}"
    

def _build_candidate_table(stars_a, stars_b, kvector, target_angle):
    table = dict()
    
    bot, top = _get_range(kvector, target_angle)
    
    for i in range(bot, top + 1):
        table[stars_a[i]] = stars_b[i]
        table[stars_b[i]] = stars_a[i]
    
    return table


def _get_candidates(stars_a, stars_b, angles, target_angle):
    candidates = []
    
    for a, b, angle in zip(stars_a, stars_b, angles):
        if target_angle - 0.0001 < angle < target_angle + 0.0001:
            candidates.append((a, b))
            candidates.append((b, a))
    
    return candidates
    

def _search_catalog(catalog, star):
    for entry in catalog:
        if entry[3] == star:
            return entry    
    

def _find_stars(database, angles, catalog):
    stars_a, stars_b, kvector = database
 
    ij_angle = angles[0]
    ij_table = _build_candidate_table(stars_a, stars_b, kvector, ij_angle)
     
    ik_angle = angles[1]
    ik_table = _build_candidate_table(stars_a, stars_b, kvector, ik_angle)
    
    jk_angle = angles[3]
    jk_table = _build_candidate_table(stars_a, stars_b, kvector, jk_angle)
    
    ik_candidates = dict()
    
    for star_i in ij_table:
        if star_i in ik_table:
            star_j = ij_table[star_i]
            star_k = ik_table[star_i]
            
            if star_j in jk_table:
                if star_k == jk_table[star_j]:
                    ik_candidates[(star_i, star_k)] = star_j
                    
    ir_angle = angles[2]
    ir_table = _build_candidate_table(stars_a, stars_b, kvector, ir_angle)
    
    kr_angle = angles[5]
    kr_table = _build_candidate_table(stars_a, stars_b, kvector, kr_angle)
            
    for star_i in ir_table:
        if star_i in ik_table:
            star_r = ir_table[star_i]
            star_k = ik_table[star_i]
            
            if star_r in kr_table:
                if star_k == kr_table[star_r]:
                    if (star_i, star_k) in ik_candidates:
                        star_j = ik_candidates[(star_i, star_k)]
                        print(star_i, star_j, star_k, star_r)


def find_ijkr(ij, ik, ir, jk, jr, kr):
    ij_dict = defaultdict(set)
    ik_dict = defaultdict(set)
    jk_dict = defaultdict(set)
    ir_dict = defaultdict(set)
    jr_dict = defaultdict(set)
    kr_dict = defaultdict(set)
    
    for i, j in ij:
        ij_dict[i].add(j)
    for i, k in ik:
        ik_dict[i].add(k)
    for j, k in jk:
        jk_dict[j].add(k)
    for i, r in ir:
        ir_dict[i].add(r)
    for j, r in jr:
        jr_dict[j].add(r)
    for k, r in kr:
        kr_dict[k].add(r)

    valid_ijk = []
    for i in ij_dict:
        for j in ij_dict[i]:  # For each (i, j) in ij
            if i in ik_dict:
                for k in ik_dict[i]:  # For each (i, k) in ik
                    if j in jk_dict and k in jk_dict[j]:  # Check if (j, k) is valid
                        valid_ijk.append((i, j, k))

    valid_combinations = []
    for i, j, k in valid_ijk:
        if i in ir_dict:
            for r in ir_dict[i]:  # For each (i, r) in ir
                if j in jr_dict and r in jr_dict[j]:  # Check (j, r)
                    if k in kr_dict and r in kr_dict[k]:  # Check (k, r)
                        valid_combinations.append((i, j, k, r))

    return valid_combinations


def _find_stars_naive(stars_a, stars_b, angles, observed_angles):
    ij_candidates = _get_candidates(stars_a, stars_b, angles, observed_angles[0])
    ik_candidates = _get_candidates(stars_a, stars_b, angles, observed_angles[1])
    ir_candidates = _get_candidates(stars_a, stars_b, angles, observed_angles[2])
    jk_candidates = _get_candidates(stars_a, stars_b, angles, observed_angles[3])
    jr_candidates = _get_candidates(stars_a, stars_b, angles, observed_angles[4])
    kr_candidates = _get_candidates(stars_a, stars_b, angles, observed_angles[5])
    
    print(find_ijkr(ij_candidates, ik_candidates, ir_candidates, jk_candidates, jr_candidates, kr_candidates))
    
    

if __name__ == "__main__":
    file_path = "bright-star-catalog.tsv"
    catalog = _parse_catalog(file_path)
    catalog = _remove_close_stars(catalog, 0.2, 1000)
    stars_a, stars_b, angles = _generate_map(catalog, 15)
    
    """
    0 1 3.588141570615857 ij
    0 2 4.675149162337611 ik
    0 3 7.296114210644325 ir
    1 2 6.970843485638382 jk
    1 3 4.782969961957494 jr
    2 3 11.551305007701963 kr
    """
    
    """
    0 1 0.9980397027206693
    0 2 0.9966728321752347
    0 3 0.9919030578075672
    1 2 0.9926080396528395
    1 3 0.9965176868162392
    2 3 0.979745789501683
    """

    print(angles[0])
    print(angles[-1])
    print(len(angles))

    # B = angles[0] - 0.0001
    # M = (angles[-1] - angles[0]) / len(angles)
    
    # kvector = _kvector(angles)
    
    observed_angles = [0.9980397027206693, 0.9966728321752347, 0.9919030578075672, 0.9926080396528395, 0.9965176868162392, 0.979745789501683]
    
    _find_stars_naive(stars_a, stars_b, angles, observed_angles)
    
    # found_stars = [3485, 3605, 3598, 3551]
    # star_array = []
    # for star in found_stars:
    #     star_array.append(_search_catalog(catalog, star))
    
    # for i in range(4):
    #     for j in range(i + 1, 4):
    #         star_a = star_array[i]
    #         star_b = star_array[j]
            
    #         print(math.degrees(math.acos(_sphere_angle(star_a, star_b))))
    
    
    # for i in range(100000):
    #     angle = random.uniform(0.968147640378, 0.999961923064)
    #     _validator(kvector, angles, angle)
    
    
    
    

    
    
    
                