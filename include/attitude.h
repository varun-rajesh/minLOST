#ifndef attitude_h
#define attitude_h

#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "utils.h"
#include "types.h"
#include "linalg.h"
#include "catalog.h"

void run_attitude(identified_star_t identified_stars[], float attitude[]);

#endif