from centroider import run_centroid
from database import run_database
from star_identifier import star_identifier
from attitude import _quest
import utils

if __name__ == "__main__":
    image_name = "img_7660.bin"
    star_catalog = "bright-star-catalog.tsv"
    
    star_centers = run_centroid(image_name)
    database = run_database(star_catalog)
    star_ids, star_centers = star_identifier(database, star_centers)

    _quest(database[3], star_ids, star_centers)