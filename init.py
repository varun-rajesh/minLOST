from string import Template
import math
from scipy.optimize import minimize
from scipy.stats import linregress
import numpy as np
import sys



# UTILS

def _sphere_angle(star1, star2):
    ra1, dec1 = star1[0], star1[1]
    ra2, dec2 = star2[0], star2[1]

    cos_distance = (
        math.sin(dec1) * math.sin(dec2) +
        math.cos(dec1) * math.cos(dec2) * math.cos(ra1 - ra2)
    )
    
    return cos_distance


def _spherical_to_spatial(spherical):
    ra, dec = spherical[0], spherical[1]
    
    x = math.cos(dec) * math.cos(ra)
    y = math.cos(dec) * math.sin(ra)
    z = math.sin(dec)
    
    return x, y, z



# STAR + ANGLE CATALOG

def _load_catalog(filepath):
    stars = []

    with open(filepath, 'r') as file:
        for line in file:
            parts = line.strip().split('|')

            raj2000 = float(parts[0])
            dej2000 = float(parts[1])
            name = int(parts[2])

            magnitude = float(parts[4])

            star = [
                math.radians(raj2000),
                math.radians(dej2000),
                magnitude,
                name
            ]

            stars.append(star)

    return stars


def _remove_close_stars(star_list, minimum_angular_seperation):
    to_remove = set()
    
    for i in range(len(star_list)):
        for j in range(i + 1, len(star_list)):
            angle = _sphere_angle(star_list[i], star_list[j])
            
            if angle > math.cos(minimum_angular_seperation):
                to_remove.add(i)
                to_remove.add(j)
    
    stripped_star_list = [star for index, star in enumerate(star_list) if index not in to_remove]
    
    return stripped_star_list


def _keep_brightest(star_list, brightest_count):
    sorted_star_list = sorted(star_list, key=lambda star: star[2])
    
    return sorted_star_list[:brightest_count]


def _rename_stars(star_list):
    for i, entry in enumerate(star_list):
        star_list[i][3] = i + 1
    
    return star_list


def _append_spatial(star_list):
    for i, entry in enumerate(star_list):
        x, y, z = _spherical_to_spatial(entry)
        star_list[i] += [x, y, z]
    
    return star_list


def _generate_angle_dict(star_list, maximum_angular_seperation):
    star_pairs = []
    angles = []
    
    for i in range(len(star_list)):
        for j in range(i + 1, len(star_list)):
            angle = _sphere_angle(star_list[i], star_list[j])
            
            if angle > math.cos(maximum_angular_seperation):
                star_pairs.append((star_list[i][3], star_list[j][3]))
                angles.append(angle)
    
    sorted_indices = sorted(range(len(angles)), key=lambda i: angles[i])
    sorted_star_pairs = []
    sorted_angles = []
    
    for index in sorted_indices:
        sorted_star_pairs.append(star_pairs[index])
        sorted_angles.append(angles[index])
    
    return sorted_star_pairs, sorted_angles


def _find_in_star_list(full_star_list, star_id):
    for i in range(len(full_star_list)):
        if full_star_list[i][3] == star_id:
            return full_star_list[i]
        

def _save_star_list(star_list, filepath):
    with open(filepath, "w") as f:
        for star in star_list:
            ra, de, magnitude, star_id, spatial_x, spatial_y, spatial_z = star
            f.write(f"{{ {ra}f, {de}f, {magnitude}f, {star_id}, {{ {spatial_x}f, {spatial_y}f, {spatial_z}f }} }},\n")
    

def _save_angle_list(star_pairs, angle_list, filepath):
    with open(filepath, "w") as f:
        for (star_a, star_b), angle in zip(star_pairs, angle_list):
            f.write(f"{{ {{ {star_a}, {star_b} }}, {angle} }},\n")
    
    

def _validate_database(full_star_list, star_pairs, angles):
    for star_pair, angle in zip(star_pairs, angles):
        star_id_a, star_id_b = star_pair
        star_a_entry = _find_in_star_list(full_star_list, star_id_a)
        star_b_entry = _find_in_star_list(full_star_list, star_id_b)
        
        assert (angle == _sphere_angle(star_a_entry, star_b_entry))


def _generate_database(bright_star_catalog_filepath, num_of_stars, minimum_angular_seperation, maximum_angular_seperation, validate):
    full_star_list = _load_catalog(bright_star_catalog_filepath)
    star_list = _keep_brightest(full_star_list, 2 * num_of_stars)
    star_list = _remove_close_stars(star_list, minimum_angular_seperation)
    star_list = _keep_brightest(star_list, num_of_stars)
    
    star_list = _rename_stars(star_list)
    star_pairs, angles = _generate_angle_dict(star_list, maximum_angular_seperation)
    if (validate):
        _validate_database(full_star_list, star_pairs, angles)
        
    star_list = _append_spatial(star_list)
    
    return star_list, star_pairs, np.array(angles)



# PARAMETER EXTRACTION

def _fit_below(x, y) :
    x0, y0 = x[0] - 1, y[0]
    x -= x0
    yp = y - y0

    def _cost(b, x, y) :
        a = np.min((y - b) / x)
        return np.sum((y - a * x - b)**2)

    b = minimize(_cost, [0], args=(x, yp)).x[0]
    a = np.min((yp - b) / x)
    return a, b - a * x0 + y0


def _fit_above(x, y):
    x0, y0 = x[0] - 1, y[0]
    x -= x0
    yp = y - y0

    def _cost(b, x, y):
        a = np.max((y - b) / x)
        return np.sum((y - a * x - b)**2)

    b = minimize(_cost, [0], args=(x, yp)).x[0]
    a = np.max((yp - b) / x)
    return a, b - a * x0 + y0


def _calculate_bounding_lines(angles):
    indices = np.array(range(len(angles)))
    indices_left = indices - 1
    indices_right = indices + 1
    
    m_left, b_left = _fit_above(indices_left, angles)
    m_right, b_right = _fit_below(indices_right, angles)
       
    m_left_inverse, b_left_inverse = 1 / m_left, -b_left / m_left
    m_right_inverse, b_right_inverse = 1 / m_right, -b_right / m_right
    
    return m_left_inverse, b_left_inverse, m_right_inverse, b_right_inverse


def _calculate_twiddle(maximum_angle, twiddle_angle):
    angle_range = np.arange(0, maximum_angle + 0.1, 0.1)
    
    cos_values = np.cos(np.radians(angle_range))
    cos_values_plus = np.cos(np.radians(angle_range + twiddle_angle))
    cos_values_minus = np.cos(np.radians(angle_range - twiddle_angle))
    
    cos_deltas = np.abs(cos_values_plus - cos_values_minus) / 2
    slope, intercept, r_value, p_value, std_err = linregress(cos_values, cos_deltas)
    
    return slope, intercept



# TEMPLATE GENERATION

def _load_config(filepath):
    config_dict = dict()
    file_content = ""
    with open(filepath, "r") as f:
        file_content = f.readlines()
    
    for line in file_content:
        if "=" in line:
            key, value = line.split("=")
            key = key.strip()
            value = value.strip()

            config_dict[key] = value

    return config_dict


def _load_template(filepath):
    template = ""
    with open(filepath, "r") as f:
        template = f.read()
    
    return Template(template)


def _save_template(template_content, filepath):
    with open(filepath, "w") as f:
        f.write(template_content)
        

def _generate_template(config_filepath):
    config_dict = _load_config(config_filepath)
    
    template_filepath = config_dict["TEMPLATE_FILE"]
    template = _load_template(template_filepath)
    
    star_catalog_filepath = config_dict["BSC_FILE"]
    star_count = int(config_dict["CATALOG_STAR_COUNT"])
    star_min_angle = math.radians(float(config_dict["CATALOG_MIN_ANGLE"]))
    star_max_angle = math.radians(float(config_dict["CATALOG_MAX_ANGLE"]))
    
    star_list, star_pairs, angles = _generate_database(star_catalog_filepath, star_count, star_min_angle, star_max_angle, False)

    _save_star_list(star_list, config_dict["STAR_CATALOG_OUTPUT"])
    _save_angle_list(star_pairs, angles, config_dict["ANGLE_CATALOG_OUTPUT"])
    
    angle_catalog_length = len(angles)
    star_catalog_length = len(star_list)

    bin_search_params = _calculate_bounding_lines(angles)
    
    twiddle_angle = math.radians(float(config_dict["STAR_ANGLE_THRESHOLD"]))
    cosine_twiddle = _calculate_twiddle(star_max_angle, twiddle_angle)
    
    angle_catalog_output = "\"" + "../" + config_dict["ANGLE_CATALOG_OUTPUT"] + "\""
    star_catalog_output = "\"" + "../" + config_dict["STAR_CATALOG_OUTPUT"] + "\""
    
    
    template = template.safe_substitute(FOCAL_LENGTH_MM = config_dict["FOCAL_LENGTH_MM"],
                             PIXEL_SIZE_UM = config_dict["PIXEL_SIZE_UM"],
                             IMAGE_X = config_dict["IMAGE_X"],
                             IMAGE_Y = config_dict["IMAGE_Y"],
                             SKIP_IMAGE_METADATA = config_dict["SKIP_IMAGE_METADATA"],
                             EIGENVALUE_THRESHOLD = config_dict["EIGENVALUE_THRESHOLD"],
                             ANGLE_CATALOG_LENGTH = angle_catalog_length,
                             STAR_CATALOG_LENGTH = star_catalog_length,
                             CATALOG_MIN_ANGLE = star_min_angle,
                             CATALOG_MAX_ANGLE = star_max_angle,
                             BIN_SEARCH_EPSILON = config_dict["BIN_SEARCH_EPSILON"],
                             BIN_SEARCH_LEFT_M = bin_search_params[0],
                             BIN_SEARCH_LEFT_B = bin_search_params[1],
                             BIN_SEARCH_RIGHT_M = bin_search_params[2],
                             BIN_SEARCH_RIGHT_B = bin_search_params[3],
                             COSINE_TWIDDLE_M = cosine_twiddle[0],
                             COSINE_TWIDDLE_B = cosine_twiddle[1],
                             MAX_ID_STARS = config_dict["MAX_ID_STARS"],
                             ANGLE_CATALOG_OUTPUT = angle_catalog_output,
                             STAR_CATALOG_OUTPUT = star_catalog_output,
                             CENTROID_STD_DEV = config_dict["CENTROID_STD_DEV"],
                             STAR_MAGNITUDE_THRESHOLD = config_dict["STAR_MAGNITUDE_THRESHOLD"],
                             MISMATCH_PROB = config_dict["MISMATCH_PROB"])

    _save_template(template, config_dict["GENERATED_FILE"])


if __name__ == "__main__":
    if (len(sys.argv) < 2):
	    print("Missing config file. Usage: python init.py <config.ini>")
        _generate_template(sys.argv[1])
    
    
    