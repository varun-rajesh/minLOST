#ifndef STAR_IDENTIFIER_H
#define STAR_IDENTIFIER_H

#define FLOAT_PRECISION_CONSTANT 2.56349e-05

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "types.h"
#include "utils.h"
#include "catalog.h"
#include "parameters.h"
#include "catalog.h"

extern identified_star_t all_stars[MAX_ID_STARS];

void run_full_star_identifier(image_star_t image_stars[], identified_star_t all_stars[MAX_ID_STARS]);

#endif // STAR_IDENTIFIER_H