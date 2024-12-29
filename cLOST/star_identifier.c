#include "star_identifier.h"

star_id_parameters_t ijkr_params;
identified_star_t identified_ijkr[4];
identified_star_t all_stars[MAX_ID_STARS];

uint32_t bounded_binary_angle_search(float target_angle) {
    uint32_t left = (uint32_t) floor(LEFT_M * target_angle + LEFT_B);
    uint32_t right = (uint32_t) floor(RIGHT_M * target_angle + RIGHT_B);

    // printf("Angle: %f, left: %d, right: %d\n", target_angle, left, right);

    uint32_t closest_index = 0;
    uint32_t best_less_than_target = 0;

    while (left <= right) {
        uint32_t mid = (uint32_t)((left + right) / 2);

        if (fabs(angle_list[mid].angle - target_angle) <= BIN_SEARCH_EPSILON) {
            closest_index = mid;
            return closest_index;
        } else if (angle_list[mid].angle < target_angle) {
            best_less_than_target = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return best_less_than_target;
}

int32_t generator(int32_t current) {
    if (current > 0) {
        return -current;
    } else {
        return -current + 1;
    }
}

bool bound_check(int32_t index, float target_angle) {

    float epsilon = COSINE_TWIDDLE_M * target_angle + COSINE_TWIDDLE_B;

    if (index < 0) {
        return false;
    } else if (index >= ANGLE_CATALOG_LENGTH) {
        return false;
    } else if (angle_list[index].angle > target_angle + epsilon) {
        return false;
    } else if (angle_list[index].angle < target_angle - epsilon) {
        return false;
    } else {
        return true;
    }
}

bool can_continue_loop(uint32_t start, int32_t *offset, float target_angle) {
    int32_t current_index = start + *offset;
    int32_t next_offset = generator(*offset);
    int32_t next_index = start + next_offset;

    bool current_bound = bound_check(current_index, target_angle);
    bool next_bound = bound_check(next_index, target_angle);

    if (!current_bound && next_bound) {
        *offset = next_offset;
    }

    return (current_bound || next_bound);
}

bool check_generic(uint32_t candidate1, uint32_t candidate2, int32_t base_index, float target_angle) {
    if (candidate1 == NO_CANDIDATE_FOUND) {
        return false;
    }
    if (candidate2 == NO_CANDIDATE_FOUND) {
        return false;
    }
    


    int32_t offset = 0;

    while (1) {

        int32_t current_index = base_index + offset;

        uint32_t lower = MIN(candidate1, candidate2);
        uint32_t upper = MAX(candidate1, candidate2);

        if (bound_check(current_index, target_angle)) {
            bool lower_match = (angle_list[current_index].star[0] == lower);
            bool upper_match = (angle_list[current_index].star[1] == upper);

            if (lower_match && upper_match) {
                return true;
            }
        }

        offset = generator(offset);
        if (!can_continue_loop(base_index, &offset, target_angle)) {
            break;
        }
    }

    return false;
}

bool check_jr(uint32_t j_candidate, uint32_t r_candidate) {
    return check_generic(j_candidate, r_candidate, ijkr_params.jr_index, ijkr_params.jr_angle);
}

bool check_kr(uint32_t k_candidate, uint32_t r_candidate) {
    return check_generic(k_candidate, r_candidate, ijkr_params.kr_index, ijkr_params.kr_angle);
}

bool check_ik(uint32_t i_candidate, uint32_t k_candidate) {
    return check_generic(i_candidate, k_candidate, ijkr_params.ik_index, ijkr_params.ik_angle);
}

ijkr_t find_r_from_ijk(uint32_t i_candidate, uint32_t j_candidate, uint32_t k_candidate) {
    uint32_t r = NO_CANDIDATE_FOUND;
    bool candidate_found = NO_CANDIDATE_FOUND;

    int32_t ir_offset = 0;
    while (1) {
        int32_t ir_index = ijkr_params.ir_index + ir_offset;
        float ir_angle = ijkr_params.ir_angle;

        if (bound_check(ir_index, ir_angle)) {
            uint32_t r_candidate = NO_CANDIDATE_FOUND;
            if (angle_list[ir_index].star[0] == i_candidate) {
                r_candidate = angle_list[ir_index].star[1];
            } else if (angle_list[ir_index].star[1] == i_candidate) {
                r_candidate = angle_list[ir_index].star[0];
            }

            if (check_jr(j_candidate, r_candidate) && check_kr(k_candidate, r_candidate)) {
                bool jkr_direction = spherical_signed_area(j_candidate, k_candidate, r_candidate);
                if (jkr_direction == ijkr_params.jkr_direction) {
                    if (candidate_found != NO_CANDIDATE_FOUND) {
                        printf("Multiple candidate found\n");
                        return (ijkr_t) {MULTI_CANDIDATE_ERROR};
                    } else {
                        // printf("ijkr: %d, %d, %d, %d\n", i_candidate, j_candidate, k_candidate, r_candidate);
                        r = r_candidate;
                        candidate_found = CANDIDATE_FOUND;
                    }
                }
            }
        }

        ir_offset = generator(ir_offset);
        if (!can_continue_loop(ijkr_params.ir_index, &ir_offset, ijkr_params.ir_angle)) {
            break;
        }
    }

    return (ijkr_t) {0, 0, 0, r, candidate_found};
}

ijkr_t find_kr_from_ij(uint32_t i_candidate, uint32_t j_candidate) {
    uint32_t r = NO_CANDIDATE_FOUND;
    uint32_t k = NO_CANDIDATE_FOUND;
    bool candidate_found = NO_CANDIDATE_FOUND;

    int32_t jk_offset = 0;
    
    while (1) {
        int32_t jk_index = ijkr_params.jk_index + jk_offset;
        float jk_angle = ijkr_params.jk_angle;

        if (bound_check(jk_index, jk_angle)) {
            uint32_t k_candidate = NO_CANDIDATE_FOUND;
            if (angle_list[jk_index].star[0] == j_candidate) {
                k_candidate = angle_list[jk_index].star[1];
            } else if (angle_list[jk_index].star[1] == j_candidate) {
                k_candidate = angle_list[jk_index].star[0];
            }

            if (check_ik(i_candidate, k_candidate)) {
                
                bool ijk_direction = spherical_signed_area(i_candidate, j_candidate, k_candidate);
                if (ijk_direction == ijkr_params.ijk_direction) {
                    ijkr_t ijkr = find_r_from_ijk(i_candidate, j_candidate, k_candidate);

                    if (ijkr.r == MULTI_CANDIDATE_ERROR) {
                        return (ijkr_t) {MULTI_CANDIDATE_ERROR};
                    } else if (ijkr.valid == CANDIDATE_FOUND) {
                        if (candidate_found == CANDIDATE_FOUND) {
                            printf("Multiple candidate found\n");
                            return (ijkr_t) {MULTI_CANDIDATE_ERROR};
                        } else {
                            r = ijkr.r;
                            k = k_candidate;
                            candidate_found = CANDIDATE_FOUND;
                        }
                    }
                }
            }
        }

        jk_offset = generator(jk_offset);
        if (!can_continue_loop(ijkr_params.jk_index, &jk_offset, jk_angle)) {
            break;
        }
    }

    return (ijkr_t) {0, 0, k, r, candidate_found};
}

ijkr_t find_ijkr() {
    int32_t ij_offset = 0;

    uint32_t i = NO_CANDIDATE_FOUND;
    uint32_t j = NO_CANDIDATE_FOUND;
    uint32_t k = NO_CANDIDATE_FOUND;
    uint32_t r = NO_CANDIDATE_FOUND;

    bool candidate_found = NO_CANDIDATE_FOUND;

    while (1) {
        int32_t ij_index = ijkr_params.ij_index + ij_offset;
        float ij_angle = ijkr_params.ij_angle;

        if (bound_check(ij_index, ij_angle)) {
            for (int swap = 0; swap < 2; ++swap) {
                uint32_t i_candidate = angle_list[ij_index].star[swap];
                uint32_t j_candidate = angle_list[ij_index].star[1 - swap];

                ijkr_t ijkr = find_kr_from_ij(i_candidate, j_candidate);

                if (ijkr.valid == MULTI_CANDIDATE_ERROR) {
                    return (ijkr_t) {MULTI_CANDIDATE_ERROR};
                } else if (ijkr.valid == CANDIDATE_FOUND) {
                    if (candidate_found != NO_CANDIDATE_FOUND) {
                        printf("Multiple candidate found\n");
                        return (ijkr_t) {MULTI_CANDIDATE_ERROR};
                    } else {
                        i = i_candidate;
                        j = j_candidate;
                        k = ijkr.k;
                        r = ijkr.r;
                        candidate_found = CANDIDATE_FOUND;
                    }
                }
            }
        }

        ij_offset = generator(ij_offset);
        if (!can_continue_loop(ijkr_params.ij_index, &ij_offset, ij_angle)) {
            break;
        }
    }

    return (ijkr_t) {i, j, k, r, candidate_found};
}


bool cmp_image_stars(image_star_t a, image_star_t b) {
    if (a.camera.x != b.camera.x) {
        return false;
    } else if (a.camera.y != b.camera.y) {
        return false;
    } else {
        return true;
    }
}


void find_additional_matches(image_star_t image_stars[]) {
    for (int i = 0; i < MAX_ID_STARS; i++) {
        image_star_t image_star = image_stars[i];
        // printf("Image star[%d]: (%f, %f)\n", i, image_star.camera.x, image_star.camera.y);
        // printf("Prev catalog star: %d\n", all_stars[i].catalog_star);

        if (image_star.magnitude == 0.f) {
            continue;
        }
        
        bool skip = false;
        for (int j = 0; j < 4; j++) {
            if (cmp_image_stars(image_star, identified_ijkr[j].image_star)) {
                // printf("Skipping %d for ijkr\n", i);
                all_stars[i].catalog_star = identified_ijkr[j].catalog_star;
                all_stars[i].image_star = image_star;
                skip = true;
                break;
            }
        }

        if(skip) {
            continue;
        }

        bool abort = false;
        float angles[4];
        uint32_t start_index[4];
        for (int j = 0; j < 4; j++) {
            angles[j] = camera_angle(image_star.camera, identified_ijkr[j].image_star.camera);
            // printf("Angle[%d][%d] = %f\n", i, j, angles[j]);
            if ((angles[j] < cos(CATALOG_MAX_ANGLE)) || (angles[j] > cos(CATALOG_MIN_ANGLE))) {
                abort = true;
                break;
            }
            // printf("Angle[%d][%d] = %f\n", i, j, angles[j]);
            start_index[j] = bounded_binary_angle_search(angles[j]);
        }

        if (abort) {
            continue;
        }

        uint32_t ix_offset = 0;
        while (1) {
            uint32_t ix_index = ix_offset + start_index[0];

            uint32_t x_candidate = NO_CANDIDATE_FOUND;
            if (angle_list[ix_index].star[0] == identified_ijkr[0].catalog_star) {
                x_candidate = angle_list[ix_index].star[1];
                // printf("ix_index: %d, i_candidate: %d, x_candidate: %d\n", ix_index, angle_list[ix_index].star[0], x_candidate);
            } else if (angle_list[ix_index].star[1] == identified_ijkr[0].catalog_star) {
                x_candidate = angle_list[ix_index].star[0];
                // printf("ix_index: %d, i_candidate: %d, x_candidate: %d\n", ix_index, angle_list[ix_index].star[1], x_candidate);
            }

            bool jx_valid = check_generic(identified_ijkr[1].catalog_star, x_candidate, start_index[1], angles[1]);
            if (jx_valid) {
                bool kx_valid = check_generic(identified_ijkr[2].catalog_star, x_candidate, start_index[2], angles[2]);
                if (kx_valid) {
                    bool rx_valid = check_generic(identified_ijkr[3].catalog_star, x_candidate, start_index[3], angles[3]);
                    if (rx_valid) {
                        // printf("Identified for: %d\n", i);
                        all_stars[i].catalog_star = x_candidate;
                        all_stars[i].image_star = image_star;
                    }
                }
            }

            ix_offset = generator(ix_offset);
            if (!can_continue_loop(start_index[0], &ix_offset, angles[0])) {
                break;
            }
        }
    }
}

bool check_mismatch(coords_2d_t a_cam, coords_2d_t b_cam, coords_2d_t c_cam) {
    coords_3d_t a_spatial, b_spatial, c_spatial;
    camera_to_spatial(a_cam, &a_spatial);
    camera_to_spatial(b_cam, &b_spatial);
    camera_to_spatial(c_cam, &c_spatial);

    float ab_angle = spatial_angle(a_spatial, b_spatial);

    coords_3d_t pair_1;
    coords_3d_t pair_2;

    // a sin inner
    pair_1 = (coords_3d_t){b_spatial.x - a_spatial.x, b_spatial.y - a_spatial.y, b_spatial.z - a_spatial.z};
    pair_2 = (coords_3d_t){c_spatial.x - a_spatial.x, c_spatial.y - a_spatial.y, c_spatial.z - a_spatial.z};
    float a_cos_inner = spatial_angle(pair_1, pair_2);
    float a_sin_inner = sqrt(1 - a_cos_inner * a_cos_inner);

    // b sin inner
    pair_1 = (coords_3d_t){a_spatial.x - b_spatial.x, a_spatial.y - b_spatial.y, a_spatial.z - b_spatial.z};
    pair_2 = (coords_3d_t){c_spatial.x - b_spatial.x, c_spatial.y - b_spatial.y, c_spatial.z - b_spatial.z};
    float b_cos_inner = spatial_angle(pair_1, pair_2);
    float b_sin_inner = sqrt(1 - b_cos_inner * b_cos_inner);

    // c sin inner
    pair_1 = (coords_3d_t){a_spatial.x - c_spatial.x, a_spatial.y - c_spatial.y, a_spatial.z - c_spatial.z};
    pair_2 = (coords_3d_t){b_spatial.x - c_spatial.x, b_spatial.y - c_spatial.y, b_spatial.z - c_spatial.z};
    float c_cos_inner = spatial_angle(pair_1, pair_2);
    float c_sin_inner = sqrt(1 - c_cos_inner * c_cos_inner);

    float max_sin = MAX(MAX(a_sin_inner, b_sin_inner), c_sin_inner);

    float expected_mismatch = (((2.56349e-05 * ab_angle) / c_sin_inner) / max_sin);

    if (expected_mismatch > 0.0001) {
        // printf("Skip mismatch\n");
        return true;
    }

    return false;
}


bool setup_star_params(image_star_t image_stars[4]) {
    uint8_t index = 0;
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = i + 1; j < 4; j++) {
            float angle = camera_angle(image_stars[i].camera, image_stars[j].camera);
            if ((angle < cos(CATALOG_MAX_ANGLE)) || (angle > cos(CATALOG_MIN_ANGLE))) {
                // printf("Star pair is not in star catalog\n");
                return false;
            }

            ijkr_params.angles[index] = angle;
            ijkr_params.angle_indices[index] = bounded_binary_angle_search(angle);
            // printf("%d: Angle: %f, Index: %d\n", index, angle, ijkr_params.angle_indices[index]);
            //fflush(stdout);
            ++index;
        }
    }

    bool mismatch = check_mismatch(image_stars[0].camera, image_stars[1].camera, image_stars[2].camera);
    // mismatch = (mismatch || check_mismatch(image_stars[0].camera, image_stars[1].camera, image_stars[3].camera));
    // mismatch = (mismatch || check_mismatch(image_stars[0].camera, image_stars[2].camera, image_stars[3].camera));
    // mismatch = (mismatch || check_mismatch(image_stars[1].camera, image_stars[2].camera, image_stars[3].camera));

    if (mismatch) {
        return false;
    }

    ijkr_params.ijk_direction = cartesian_signed_area(image_stars[0].camera, image_stars[1].camera, image_stars[2].camera);
    ijkr_params.jkr_direction = cartesian_signed_area(image_stars[1].camera, image_stars[2].camera, image_stars[3].camera);

    return true;
}

void run_star_identifier(image_star_t image_stars[], uint32_t image_stars_length) {
    uint8_t min_identified_req = 4;

    for (int dj = 1; dj <= image_stars_length - 3; dj++) {
        for (int dk = 1; dk <= image_stars_length - dj - 2; dk++) {
            for (int dr = 1; dr <= image_stars_length - dj - dk - 1; dr++) {
                for (int i = 1; i <= image_stars_length - dj - dk - dr; i++) {
                    int j = i + dj;
                    int k = j + dk;
                    int r = k + dr;

                    image_star_t candidates[4] = {
                        image_stars[i - 1],
                        image_stars[j - 1],
                        image_stars[k - 1],
                        image_stars[r - 1]
                    };

                    for (uint8_t i = 0; i < 4; i++) {
                        if (candidates[i].magnitude == 0.f) {
                            // printf("Rejected: (%f, %f, %f)\n", candidates[i].camera.x, candidates[i].camera.y, candidates[i].magnitude);
                            continue;
                        }
                    }

                    for (int i = 0; i < MAX_ID_STARS; i++) {
                        all_stars[i].catalog_star = 0;
                        all_stars[i].image_star.magnitude = 0.0f;
                    }

                    // printf("ijkr: %d %d %d %d\n", i - 1, j - 1, k - 1, r - 1);

                    bool valid_setup = setup_star_params(candidates);
                    if (valid_setup) {
                        ijkr_t ijkr = find_ijkr();

                        if (ijkr.valid) {
                            identified_ijkr[0].image_star = image_stars[i - 1];
                            identified_ijkr[0].catalog_star = ijkr.i;
                            identified_ijkr[1].image_star = image_stars[j - 1];
                            identified_ijkr[1].catalog_star = ijkr.j;
                            identified_ijkr[2].image_star = image_stars[k - 1];
                            identified_ijkr[2].catalog_star = ijkr.k;
                            identified_ijkr[3].image_star = image_stars[r - 1];
                            identified_ijkr[3].catalog_star = ijkr.r;

                            find_additional_matches(image_stars);

                            uint8_t total_identified = 0;
                            for (int i = 0; i < MAX_ID_STARS; i++) {
                                if (all_stars[i].catalog_star != 0) {
                                    total_identified += 1;
                                }
                            }

                            return;
                        }                      
                    }

                }
            }
        }
    }
}

void run_full_star_identifier(image_star_t image_stars[]) {

    for (int i = 0; i < MAX_ID_STARS; i++) {
        all_stars[i].catalog_star = 0;
        all_stars[i].image_star.magnitude = 0.0f;
    }

    uint8_t identified_star_count = 0;
    for (int i = 0; i < MAX_ID_STARS; i++) {
        if (image_stars[i].magnitude != 0.f) {
            identified_star_count += 1;
        }
    }

    if (identified_star_count < 4) {
        timer_stop_discard();
        return;
    }

    run_star_identifier(image_stars, identified_star_count);
}

// int main() {
//     initialize_catalog("angle_catalog.csv", "star_catalog.csv");

//     image_star_t image_stars[MAX_ID_STARS] = {
//         {{437.878723, 510.500000}, 940.000000},
//         {{413.835663, 509.500000}, 858.000000},
//         {{461.906036, 511.500000}, 830.000000},
//         {{503.581604, 545.500000}, 576.000000},
//         {{479.465424, 544.500000}, 434.000000},
//         {{552.560913, 188.500000}, 427.000000},
//         {{576.544128, 189.500000}, 408.000000},
//         {{389.613861, 508.500000}, 404.000000},
//         {{527.923218, 546.500000}, 241.000000},
//         {{485.500000, 512.500000}, 99.000000},
//         {{600.500000, 190.500000}, 96.000000},
//         {{528.500000, 187.500000}, 96.000000},
//         {{0.000000, 0.000000}, 0.000000},
//         {{0.000000, 0.000000}, 0.000000},
//         {{0.000000, 0.000000}, 0.000000},
//         {{0.000000, 0.000000}, 0.000000},
//     };

//     run_star_identifier(image_stars, MAX_ID_STARS);
//     find_additional_matches(image_stars, MAX_ID_STARS);

//     for (int i = 0; i < MAX_ID_STARS; i++) {
//         float x = all_stars[i].image_star.camera.x;
//         float y = all_stars[i].image_star.camera.y;
//         uint32_t id = all_stars[i].catalog_star;

//         printf("\t\t\t\t{ { { %f, %f }, 0.0 }, %d },\n", x, y, id);
//     }
// }

