#ifndef attitude_h
#define attitude_h

#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "utils.h"
#include "types.h"
#include "linalg.h"
#include "catalog.h"

void quest(catalog_entry_t star_catalog[], uint32_t star_ids[], coords_2d_t star_coords[], float quaternion[], uint8_t n);

#endif