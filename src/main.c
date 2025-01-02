#include "attitude.h"
#include "centroider.h"
#include "star_identifier.h"
#include "catalog.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return -1;
    }

    const char *input_file = argv[1];

    image_star_t image_stars[MAX_ID_STARS] = {0};
    run_centroider(input_file, image_stars);

    identified_star_t all_stars[MAX_ID_STARS] = {0};
    run_full_star_identifier(image_stars, all_stars);

    float attitude[4] = {0.f};
    run_attitude(all_stars, attitude);

    return 0;
}