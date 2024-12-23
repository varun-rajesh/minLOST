#ifndef attitude_h
#define attitude_h

#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "utils.h"
#include "types.h"
#include "linalg.h"
#include "catalog.h"

void quest(identified_star_t identifed_stars[], float quaternion[], uint8_t n);

#endif