import utils
import copy
from PIL import Image
import sys

def _image_threshold(image, image_height, image_width, image_size):
    assert(image_height * image_width == image_size)
    
    avg_intensity = 0
    for y in range(image_height):
        for x in range(image_width):
            avg_intensity += image[y][x]
    
    avg_intensity = avg_intensity / image_size
    
    std_intensity = 0
    for y in range(image_height):
        for x in range(image_width):
            std_intensity += (image[y][x] - avg_intensity) ** 2
    
    std_intensity = (std_intensity / image_size) ** 0.5
    
    return avg_intensity, std_intensity


def _calculate_threshold(avg_intensity, std_intensity):
    return avg_intensity + 5 * std_intensity


# This is not part of the LOST implementation
def _reduce_noise(image, image_height, image_width, average):
    for y in range(image_height):
        for x in range(image_width):
            image[y][x] -= average
            

def _center_of_gravity(image, image_height, image_width, image_size, threshold):
    visited = []
    for row in range(image_height):
        visited.append([False] * image_width)
        
    def _cog_helper(visited_pixels, x, y):       
        if visited_pixels[y][x]:
            return 0, 0, 0, True
        if (x < 0) or (x >= image_width):
            return 0, 0, 0, True
        if (y < 0) or (y >= image_height):
            return 0, 0, 0, True
        if (x == 0) or (x == image_width - 1):
            return 0, 0, 0, False
        if (y == 0) or (y == image_height - 1):
            return 0, 0, 0, False
        if image[y][x] < threshold:
            return 0, 0, 0, True

        visited_pixels[y][x] = True    
        
        cum_x = x * image[y][x]
        cum_y = y * image[y][x]
        count = image[y][x]
        
        directions = [(0, 1), (0, -1), (1, 0), (-1, 0)]
        
        for dx, dy in directions:
            res_x, res_y, res_count, res_valid = _cog_helper(visited_pixels, x + dx, y + dy)
            if not res_valid:
                return 0, 0, 0, False
            else:
                cum_x += res_x
                cum_y += res_y
                count += res_count
        
        return cum_x, cum_y, count, True
    
    centers = []           
    for y in range(image_height):
        for x in range(image_width):
            cum_x, cum_y, count, valid = _cog_helper(visited, x, y)
            
            if valid and count > 0:
                centers.append((int(cum_x / count), int(cum_y / count), count))
                
    return centers
    

def run_centroid(filename):
    image, height, width, size = utils.import_binary(filename)
    avg, std = _image_threshold(image, height, width, size)
    threshold = _calculate_threshold(avg, std)
    centers = _center_of_gravity(image, height, width, size, threshold)
    
    return sorted(centers, key=lambda i: i[2], reverse=True)


if __name__ == "__main__":
    image, height, width, size = utils.import_binary("img_7660.bin")
    avg, std = _image_threshold(image, height, width, size)
    thresh = _calculate_threshold(avg, std)
    
    centers = _center_of_gravity(image, height, width, size, thresh)

    sorted_centers = sorted(centers, key=lambda i: i[2], reverse=True)
    
    for i in range(4):
        for j in range(i + 1, 4):
            
            print(i, j, utils.angle(sorted_centers[i], sorted_centers[j]))
