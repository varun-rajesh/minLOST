#include "utils.h"
#include "types.h"

void camera_to_spatial(float pixel[], float spatial[]) {
    spatial[0] = 1.f;
    spatial[1] = (-pixel[0] + IMAGE_HALF_X) / FOCAL_LENGTH_PIXELS;
    spatial[2] = (-pixel[1] + IMAGE_HALF_Y) / FOCAL_LENGTH_PIXELS;
}