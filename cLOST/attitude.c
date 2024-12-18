#include "attitude.h"

void quest(catalog_entry_t star_catalog[], uint16_t star_ids[], float star_coords[][2], uint8_t n) {
    
    float B[9] = {0.f};

    for (uint8_t i = 0; i < n; ++i) {
        float cam_spatial[3] = {0.f};
        camera_to_spatial(star_coords[i], cam_spatial);

        catalog_entry_t star = star_catalog[star_ids[i]];
        float cat_spatial[3] = {star.x, star.y, star.z};

        float outer[9] = {0.f};
        outer_product(cat_spatial, cam_spatial, outer, 3);
        add(B, outer, B, 9);
    }

    float sigma = trace(B, 3);
    float Z[3] = {B[5] - B[7], B[6] - B[2], B[1] - B[3]};

    float B_T[9] = {0.f};
    float S[9] = {0.f};
    transpose(B, B_T, 3);
    add(B, B_T, S, 9);

    float delta = det3(S);

    float S_INV[9] = {0.f};
    inverse3(S, S_INV);
    scalar_multiply(S_INV, delta, S_INV, 9);
    float kappa = trace(S_INV, 3);

    float a = sigma * sigma - kappa;
    float b = sigma * sigma + dot(Z, Z, 3);
    float c = delta + change_of_base(Z, S, 3);

    float S_2[9] = {0.f};
    matrix_multiply(S, S, S_2, 3);
    float d = change_of_base(Z, S_2, 3);

    printf("a: %f, b: %f, c: %f, d: %f \n", a, b, c, d);
}

int main() {
    // Create an array to store 4 catalog entries
    catalog_entry_t catalog[4] = {
        {0.24743677667851524, 1.059705749986988, 2.47, 0.0f, 0.0f, 0.0f},
        {0.37444585242271433, 1.051303926954105, 2.68, 0.0f, 0.0f, 0.0f},
        {0.17675094197274493, 0.9867606738309735, 2.23, 0.0f, 0.0f, 0.0f},
        {0.4991423494547283, 1.1112511347447898, 3.38, 0.0f, 0.0f, 0.0f}
    };

    // Calculate the x, y, z coordinates for each entry
    for (int i = 0; i < 4; ++i) {
        catalog[i].x = cos(catalog[i].de) * cos(catalog[i].ra);
        catalog[i].y = cos(catalog[i].de) * sin(catalog[i].ra);
        catalog[i].z = sin(catalog[i].de);
    }

    uint16_t star_ids[] = {0, 1, 2, 3};
    float star_coords[][2] = {{547, 448}, {409, 437}, {609, 619}, {328, 271}};

    quest(catalog, star_ids, star_coords, 4);

    return 0;
}