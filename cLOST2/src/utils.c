#include "utils.h"

uint8_t rgb_to_grayscale(const uint8_t rgb[3]) {
    return (uint8_t)(0.299 * rgb[0] + 0.587 * rgb[1] + 0.114 * rgb[2]);
}