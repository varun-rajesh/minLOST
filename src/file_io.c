#include "file_io.h"

void import_binary_image(uint8_t image[IMAGE_Y][IMAGE_X], const char *filepath) {
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

            image[row][col] = rgb_to_grayscale(rgb);
        }
    }

    fclose(file);
}