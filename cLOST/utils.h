#ifndef utils_h
#define utils_h

#include <stdint.h>
#include <math.h>
#include "parameters.h"
#include <assert.h> 
#include "types.h"
#include "linalg.h"
#include "catalog.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void camera_to_spatial(coords_2d_t pixel, coords_3d_t *spatial);

float camera_angle(coords_2d_t a, coords_2d_t b);

bool spherical_signed_area(uint32_t a_index, uint32_t b_index, uint32_t c_index);

bool cartesian_signed_area(coords_2d_t a, coords_2d_t b, coords_2d_t c);

#endif 