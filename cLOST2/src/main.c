#include "centroider.h"

int main() {
    image_star_t image_stars[MAX_ID_STARS];
    run_centroider("raw-input-37.bin", image_stars);

    for (int i = 0; i < MAX_ID_STARS; i++) {
        if (image_stars[i].magnitude != 0) {
            printf("Star at: (%f, %f, %d)\n", image_stars[i].camera.x, image_stars[i].camera.y, image_stars[i].magnitude);
        }
    }
}   