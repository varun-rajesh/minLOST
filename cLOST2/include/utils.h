#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define POW2(a) ((a) * (a))
#define POW3(a) ((a) * (a) * (a))

uint8_t rgb_to_grayscale(const uint8_t rgb[3]);

#endif // UTILS_H