#ifndef PARAMETERS_H
#define PARAMETERS_H


#define FOCAL_LENGTH_MM 6
#define PIXEL_SIZE_UM 3.45
#define FOCAL_LENGTH_PIXELS ((FOCAL_LENGTH_MM * 1000.f)/ PIXEL_SIZE_UM)

#define IMAGE_X 1456
#define IMAGE_HALF_X (IMAGE_X / 2.f)

#define IMAGE_Y 1088
#define IMAGE_HALF_Y (IMAGE_Y / 2.f)

#define SKIP_IMAGE_METADATA 1

#define CENTROID_STD_DEV 5

#define STAR_MAGNITUDE_THRESHOLD 300

#define EIGENVALUE_THRESHOLD 0.00001

#define ANGLE_CATALOG_LENGTH 934
#define STAR_CATALOG_LENGTH 100
#define ANGLE_CATALOG_CSV angle_catalog.csv
#define STAR_CATALOG_CSV star_catalog.csv

#define CATALOG_MIN_ANGLE 0.2
#define CATALOG_MAX_ANGLE 50.0

#define BIN_SEARCH_EPSILON 0.000001
#define LEFT_M 2568.2625691677267
#define LEFT_B -1682.4041957131676
#define RIGHT_M 2599.5384653976407
#define RIGHT_B -1664.8104158379583

#define NO_CANDIDATE_FOUND 0
#define CANDIDATE_FOUND 1
#define MULTI_CANDIDATE_ERROR 2147483647

#define COSINE_TWIDDLE_M -0.0034879920230208353
#define COSINE_TWIDDLE_B 0.0037757760125690515

#define MAX_ID_STARS 16

#endif // PARAMETERS_H