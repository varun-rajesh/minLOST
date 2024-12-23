#include "catalog.h"

angle_entry_t angle_list[ANGLE_CATALOG_LENGTH];
catalog_entry_t star_list[STAR_CATALOG_LENGTH];

void populate_angle_list(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    char line[256];
    size_t index = 0;

    while (fgets(line, sizeof(line), file) && index < ANGLE_CATALOG_LENGTH) {
        line[strcspn(line, "\n")] = '\0';

        uint32_t star0, star1;
        float angle;

        if (sscanf(line, "%u,%u,%f", &star0, &star1, &angle) == 3) {
            angle_list[index].star[0] = star0;
            angle_list[index].star[1] = star1;
            angle_list[index].angle = angle;
            index++;
        } else {
            fprintf(stderr, "Skipping malformed line: %s\n", line);
        }
    }

    if (index != ANGLE_CATALOG_LENGTH) {
        fprintf(stderr, "Warning: Expected %zu entries but read %zu entries.\n", ANGLE_CATALOG_LENGTH, index);
    }

    fclose(file);
}

void populate_star_list(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    char line[256];
    size_t index = 0;

    while (fgets(line, sizeof(line), file) && index < STAR_CATALOG_LENGTH) {
        line[strcspn(line, "\n")] = '\0';
        float ra, de, magnitude, x, y, z;
        int32_t id;

        if (sscanf(line, "%f,%f,%f,%d,%f,%f,%f", &ra, &de, &magnitude, &id, &x, &y, &z) == 7) {
            star_list[index].ra = ra;
            star_list[index].de = de;
            star_list[index].magnitude = magnitude;
            star_list[index].id = id;
            star_list[index].spatial.x = x;
            star_list[index].spatial.y = y;
            star_list[index].spatial.z = z;
            index++;
        } else {
            fprintf(stderr, "Skipping malformed line: %s\n", line);
        }
    }

    if (index != STAR_CATALOG_LENGTH) {
        fprintf(stderr, "Warning: Expected %d entries but read %zu entries.\n", STAR_CATALOG_LENGTH, index);
    }

    fclose(file);
}

void initialize_catalog(const char *angle_filename, const char *star_filename) {
    populate_angle_list(angle_filename);
    populate_star_list(star_filename);
}