#ifndef attitude_h
#define attitude_h

#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "utils.h"
#include "types.h"
#include "linalg.h"
#include "timer.h"
#include "catalog.h"

float quest_char_poly(float x, float a, float b, float c, float d, float s);
float quest_char_poly_prime(float x, float a, float b, float c);
float quest_eigenvalue_estimator(float guess, float a, float b, float c, float d, float s, float epsilon);
void quaternion_to_euler(const float q[4], float *ra, float *de, float *roll);
int assign_weights(identified_star_t identified_stars[]);
void quest(identified_star_t identifed_stars[], float quaternion[], uint8_t n);
void run_attitude(identified_star_t identified_stars[], float attitude[]);

#endif