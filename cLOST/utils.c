#include "utils.h"


void camera_to_spatial(coords_2d_t pixel, coords_3d_t *spatial) {
    spatial->x = 1.f;
    spatial->y = (-pixel.x + IMAGE_HALF_X) / FOCAL_LENGTH_PIXELS;
    spatial->z = (-pixel.y + IMAGE_HALF_Y) / FOCAL_LENGTH_PIXELS;
}

float camera_angle(coords_2d_t a, coords_2d_t b) {
    coords_3d_t spatial_a;
    coords_3d_t spatial_b;

    camera_to_spatial(a, &spatial_a);
    camera_to_spatial(b, &spatial_b);

    return spatial_angle(spatial_a, spatial_b);
}

float spatial_angle(coords_3d_t spatial_a, coords_3d_t spatial_b) {
    float dot_product = dot_coords(spatial_a, spatial_b);
   
    float mag_a = sqrt(dot_coords(spatial_a, spatial_a));
    float mag_b = sqrt(dot_coords(spatial_b, spatial_b));

    float cos_theta = dot_product / (mag_a * mag_b);

    cos_theta = MAX(-1.0, MIN(1.0, cos_theta));

    return cos_theta;
}

bool spherical_signed_area(uint32_t a_index, uint32_t b_index, uint32_t c_index) {
    coords_3d_t a = star_list[a_index - 1].spatial;
    assert(a_index == star_list[a_index - 1].id);
    coords_3d_t b = star_list[b_index - 1].spatial;
    assert(b_index == star_list[b_index - 1].id);
    coords_3d_t c = star_list[c_index - 1].spatial;
    assert(c_index == star_list[c_index - 1].id);


    coords_3d_t ab = {b.x - a.x, b.y - a.y, b.z - a.z};
    coords_3d_t ac = {c.x - a.x, c.y - a.y, c.z - a.z};

    coords_3d_t cross = cross_product(ab, ac);

    float orientation = dot_coords(a, cross);

    return orientation > 0;
}

bool cartesian_signed_area(coords_2d_t a, coords_2d_t b, coords_2d_t c) {
    float area = (a.x * (b.y - c.y)) +
                 (b.x * (c.y - a.y)) +
                 (c.x * (a.y - b.y));

    return area > 0;
}
