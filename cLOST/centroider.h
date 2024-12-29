#ifndef centroider_h
#define centroider_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "parameters.h"
#include "types.h"
#include "timer.h"
#include "utils.h"

extern image_star_t image_stars[MAX_ID_STARS];

void run_centroider(const char *filename);

#endif