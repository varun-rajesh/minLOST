#include "centroider.h"

void calculate_threshold_params(const uint8_t image[IMAGE_Y][IMAGE_X], float *avg_intensity, float *std_intensity) {
    float avg = 0.f;
    for (uint32_t row = 0; row < IMAGE_Y; row++) {
        for (uint32_t col = 0; col < IMAGE_X; col++) {
            avg += image[row][col];
        }
    }

    avg = avg / (IMAGE_X * IMAGE_Y);
    *avg_intensity = avg;

    float std = 0.f;
    for (uint32_t row = 0; row < IMAGE_Y; row++) {
        for (uint32_t col = 0; col < IMAGE_X; col++) {
            float diff = image[row][col] - avg;
            std += diff * diff;
        }
    }
    *std_intensity = sqrt(std / (IMAGE_X * IMAGE_Y));
}

uint8_t calculate_threshold(float avg_intensity, float std_intensity) {
    return (uint8_t) (avg_intensity + CENTROID_STD_DEV * std_intensity);
}

bool identify_star_helper(uint8_t image[IMAGE_Y][IMAGE_X], image_star_t image_stars[MAX_ID_STARS], int x, int y, uint32_t *x_weight, uint32_t *y_weight, uint32_t *weight, uint8_t threshold) {
    if ((x < 0) || (x >= IMAGE_X)) {
        return true;
    }
    if ((y < 0) || (y >= IMAGE_Y)) {
        return true; 
    }
    if ((image[y][x] < threshold)) {
        return true;
    } else {
        if ((x == 0) || (x == IMAGE_X - 1)) {
            return false;
        }
        if ((y == 0) || (y == IMAGE_Y - 1)) {
            return false;
        }
    }

    uint32_t curr_x_weight = x * image[y][x];
    uint32_t curr_y_weight = y * image[y][x];
    uint32_t curr_weight = image[y][x];

    image[y][x] = 0.f;

    int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    for(int dir = 0; dir < 4; dir++) {
        int new_x = x + directions[dir][0];
        int new_y = y + directions[dir][1];

        uint32_t helper_x_weight = 0;
        uint32_t helper_y_weight = 0;
        uint32_t helper_weight = 0;
        
        bool valid = identify_star_helper(image, image_stars, new_x, new_y, &helper_x_weight, &helper_y_weight, &helper_weight, threshold);

        if (valid) {
            curr_x_weight += helper_x_weight;
            curr_y_weight += helper_y_weight;
            curr_weight += helper_weight;
        } else {
            return false;
        }
    }

    *x_weight = curr_x_weight;
    *y_weight = curr_y_weight;
    *weight = curr_weight;
    image[y][x] = 0.f;

    return true;
}

int smallest_magnitude_in_image_stars(image_star_t image_stars[MAX_ID_STARS]) {
    int smallest = 0;
    uint32_t min_magnitude = image_stars[0].magnitude;

    for (int i = 1; i < MAX_ID_STARS; i++) {
        if (image_stars[i].magnitude < min_magnitude) {
            min_magnitude = image_stars[i].magnitude;
            smallest = i;
        }
    }

    return smallest;
}

void identify_stars(uint8_t image[IMAGE_Y][IMAGE_X], image_star_t image_stars[MAX_ID_STARS], uint8_t threshold) {
    for (int i = 0; i < MAX_ID_STARS; i++) {
        image_stars[i].magnitude = 0.f;
    }

    for (int row = 0; row < IMAGE_Y; row++) {
        for (int col = 0; col < IMAGE_X; col++) {
            uint32_t curr_x_weight = 0;
            uint32_t curr_y_weight = 0;
            uint32_t curr_weight = 0;

            bool valid = identify_star_helper(image, image_stars, col, row, &curr_x_weight, &curr_y_weight, &curr_weight, threshold);

            if (valid && (curr_weight > STAR_MAGNITUDE_THRESHOLD)) {
                float normalized_curr_x_weight = ((float) curr_x_weight / (float) curr_weight) + 0.5;
                float normalized_curr_y_weight = ((float) curr_y_weight / (float) curr_weight) + 0.5;

                int smallest_star = smallest_magnitude_in_image_stars(image_stars);
                uint32_t magnitude = image_stars[smallest_star].magnitude;

                if (curr_weight > magnitude) {
                    image_stars[smallest_star] = (image_star_t) {{normalized_curr_x_weight, normalized_curr_y_weight}, curr_weight};
                }
            }
        }
    }
}

int compare_image_stars(const void *a, const void *b) {
    const image_star_t *star_a = (const image_star_t *) a;
    const image_star_t *star_b = (const image_star_t *) b;

    if ((star_a->magnitude > 0) && (star_b->magnitude == 0)) return -1;
    if ((star_a->magnitude == 0) && (star_b->magnitude > 0)) return 1;

    float star_a_distance = POW2(star_a->camera.x - IMAGE_HALF_X) + POW2(star_a->camera.y - IMAGE_HALF_Y);
    float star_b_distance = POW2(star_b->camera.x - IMAGE_HALF_X) + POW2(star_b->camera.y - IMAGE_HALF_Y);

    if (star_a_distance > star_b_distance) return 1;
    if (star_b_distance > star_a_distance) return -1;
    return 0;
}

void run_centroider(const char *filename, image_star_t image_stars[MAX_ID_STARS]) {
    uint8_t image[IMAGE_Y][IMAGE_X];
    printf("Importing image\n");
    import_binary_image(image, filename);

    float avg_intensity, std_intensity;
    calculate_threshold_params(image, &avg_intensity, &std_intensity);

    uint8_t threshold = calculate_threshold(avg_intensity, std_intensity);
    identify_stars(image, image_stars, threshold);
    qsort(image_stars, MAX_ID_STARS, sizeof(image_star_t), compare_image_stars);
}