#ifndef PARAMETERS_H
#define PARAMETERS_H

// Focal length and pixel size parameters
#define FOCAL_LENGTH_MM 6
#define PIXEL_SIZE_UM 3.45
#define FOCAL_LENGTH_PIXELS ((FOCAL_LENGTH_MM * 1000.f) / PIXEL_SIZE_UM)

// Image dimensions and derived values
#define IMAGE_X 1456
#define IMAGE_HALF_X (IMAGE_X / 2.f)
#define IMAGE_Y 1088
#define IMAGE_HALF_Y (IMAGE_Y / 2.f)

// Centroiding parameters
#define CENTROID_STD_DEV 5
#define STAR_MAGNITUDE_THRESHOLD 300

// Image metadata handling
#define SKIP_IMAGE_METADATA 1

// Eigenvalue threshold
#define EIGENVALUE_THRESHOLD 0.00001

// Catalog lengths and output paths
#define ANGLE_CATALOG_LENGTH 94026
#define STAR_CATALOG_LENGTH 1000
#define ANGLE_CATALOG_OUTPUT ".././database/angle_catalog.txt"
#define STAR_CATALOG_OUTPUT ".././database/star_catalog.txt"

// Catalog angle limits
#define CATALOG_MIN_ANGLE 0.003490658503988659
#define CATALOG_MAX_ANGLE 0.8726646259971648

// Binary search parameters
#define BIN_SEARCH_EPSILON 0.000001
#define LEFT_M 259620.62708441378
#define LEFT_B -167946.07836505712
#define RIGHT_M 262726.0048671042
#define RIGHT_B -168696.90545108254

// Candidate identification status
#define NO_CANDIDATE_FOUND 0
#define CANDIDATE_FOUND 1
#define MULTI_CANDIDATE_ERROR 2147483647

// Cosine twiddle factors
#define COSINE_TWIDDLE_M -0.0035944178231249475
#define COSINE_TWIDDLE_B 0.0035945010393266257

// Star identification parameters
#define MISMATCH_PROB 0.0001
#define MAX_ID_STARS 8

#endif // PARAMETERS_H
