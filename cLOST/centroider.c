#include "centroider.h"

uint8_t image[IMAGE_Y][IMAGE_X];
image_star_t image_stars[MAX_ID_STARS];

void import_binary_image(const char *filepath) {
    FILE *file = fopen(filepath, "rb");

    if (!file) {
        printf("Failed to open %s\n", filepath);
    }

    #if SKIP_IMAGE_METADATA == 1
        fseek(file, 8, SEEK_SET);
    #endif

    uint8_t rgb[3];
    for (uint32_t row = 0; row < IMAGE_Y; row++) {
        for (uint32_t col = 0; col < IMAGE_X; col++) {
            if (fread(rgb, sizeof(uint8_t), 3, file) != 3) {
                printf("Unexpected EOF in %s\n", filepath);
                fclose(file);
            }

            image[row][col] = (uint8_t)(0.299 * rgb[0] + 0.587 * rgb[1] + 0.114 * rgb[2]);
        }
    }

    fclose(file);
}

void set_threshold_params(float *avg_intensity, float *std_intensity) {
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

float calculate_threshold(float avg_intensity, float std_intensity) {
    return avg_intensity + 5 * std_intensity;
}

bool identify_star_helper(int x, int y, float *x_weight, float *y_weight, float *weight, float threshold) {
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

    // printf("Star at: (%d, %d)\n", x, y);

    float curr_x_weight = x * image[y][x];
    float curr_y_weight = y * image[y][x];
    float curr_weight = image[y][x];

    image[y][x] = 0.f;

    int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    for(int dir = 0; dir < 4; dir++) {
        int new_x = x + directions[dir][0];
        int new_y = y + directions[dir][1];

        float helper_x_weight = 0.f;
        float helper_y_weight = 0.f;
        float helper_weight = 0.f;
        
        bool valid = identify_star_helper(new_x, new_y, &helper_x_weight, &helper_y_weight, &helper_weight, threshold);

        //printf("Checking at: (%d, %d) -> %d\n", new_x, new_y, valid);

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

int smallest_magnitude_in_image_stars() {
    int smallest = 0;
    float min_magnitude = image_stars[0].magnitude;

    for (int i = 1; i < MAX_ID_STARS; i++) {
        if (image_stars[i].magnitude < min_magnitude) {
            min_magnitude = image_stars[i].magnitude;
            smallest = i;
        }
    }

    return smallest;
}

void identify_stars(float threshold) {
    for (int i = 0; i < MAX_ID_STARS; i++) {
        image_stars[i].magnitude = 0.f;
    }

    for (int row = 0; row < IMAGE_Y; row++) {
        for (int col = 0; col < IMAGE_X; col++) {
            float curr_x_weight = 0.f;
            float curr_y_weight = 0.f;
            float curr_weight = 0.f;

            bool valid = identify_star_helper(col, row, &curr_x_weight, &curr_y_weight, &curr_weight, threshold);

            if (valid && (curr_weight > 300.f)) {
                // printf("Star at: (%d, %d)\n", col, row);
                curr_x_weight = (curr_x_weight / curr_weight) + 0.5;
                curr_y_weight = (curr_y_weight / curr_weight) + 0.5;

                int smallest_star = smallest_magnitude_in_image_stars();
                float magnitude = image_stars[smallest_star].magnitude;

                if (curr_weight > magnitude) {
                    image_stars[smallest_star] = (image_star_t) {{curr_x_weight, curr_y_weight}, curr_weight};
                }
            }
        }
    }
}


int compare_image_stars(const void *a, const void *b) {
    const image_star_t *star_a = (const image_star_t *)a;
    const image_star_t *star_b = (const image_star_t *)b;

    if ((star_a->magnitude > 0) && (star_b->magnitude == 0)) return -1;
    if ((star_a->magnitude == 0) && (star_b->magnitude > 0)) return 1;

    float star_a_distance = POW2(star_a->camera.x - IMAGE_HALF_X)+ POW2(star_a->camera.y - IMAGE_HALF_Y);
    float star_b_distance = POW2(star_b->camera.x - IMAGE_HALF_X) + POW2(star_b->camera.y - IMAGE_HALF_Y);

    if (star_a_distance > star_b_distance) return 1;
    if (star_b_distance > star_a_distance) return -1;
    return 0;
}

void run_centroider(const char *filename) {
    import_binary_image(filename);

    timer_start();
    float avg_intensity, std_intensity;
    set_threshold_params(&avg_intensity, &std_intensity);
    float threshold = calculate_threshold(avg_intensity, std_intensity);
    identify_stars(threshold);
    qsort(image_stars, MAX_ID_STARS, sizeof(image_star_t), compare_image_stars);
    timer_centroid();

    for (int i = 0; i < MAX_ID_STARS; i++) {
        if (image_stars[i].magnitude != 0.f) {
            // printf("Star at: (%f, %f, %f)\n", image_stars[i].camera.x, image_stars[i].camera.y, image_stars[i].magnitude);
        }
    }
}


// int main() {
//     import_binary_image("raw-input.bin");

//     float avg_intensity, std_intensity;
//     set_threshold_params(&avg_intensity, &std_intensity);
//     float threshold = calculate_threshold(avg_intensity, std_intensity);
//     identify_stars(threshold);
//     qsort(image_stars, MAX_ID_STARS, sizeof(image_star_t), compare_image_stars);

//     for (int i = 0; i < MAX_ID_STARS; i++) {
//         printf("{{%f, %f}, %f},\n", image_stars[i].camera.x, image_stars[i].camera.y, image_stars[i].magnitude);
//     }

//     return 0;
// }

