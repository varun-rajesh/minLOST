#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include "parameters.h"

typedef struct {
    float x;
    float y;
} coords_2d_t;

typedef struct {
    float x;
    float y;
    float z;
} coords_3d_t;

typedef struct {
    float ra;
    float de;
    float magnitude;
    uint32_t id;
    coords_3d_t spatial;
} catalog_entry_t;

typedef struct {
    uint32_t star[2];
    float angle;
} angle_entry_t;

typedef struct {
    union {
        float angles[6];
        struct {
            float ij_angle;
            float ik_angle;
            float ir_angle;
            float jk_angle;
            float jr_angle;
            float kr_angle;
        };
    };
    union {
        uint32_t angle_indices[6];
        struct {
            uint32_t ij_index;
            uint32_t ik_index;
            uint32_t ir_index;
            uint32_t jk_index;
            uint32_t jr_index;
            uint32_t kr_index;
        };
    };
    bool ijk_direction;
    bool jkr_direction;
} star_id_parameters_t;


typedef struct {
    uint32_t i;
    uint32_t j;
    uint32_t k;
    uint32_t r;
    uint32_t valid;
} ijkr_t;


typedef struct {
    coords_2d_t camera;
    uint32_t magnitude;
} image_star_t;


typedef struct {
    image_star_t image_star;
    uint32_t catalog_star;
} identified_star_t;


#endif
