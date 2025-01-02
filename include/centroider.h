#ifndef CENTROIDER_H
#define CENTROIDER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "parameters.h"
#include "utils.h"
#include "types.h"
#include "file_io.h"

void run_centroider(const char *filename, image_star_t image_stars[MAX_ID_STARS]);

#endif