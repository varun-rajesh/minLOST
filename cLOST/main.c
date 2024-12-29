// #include "attitude.h"
// #include "centroider.h"
// #include "star_identifier.h"
// #include "catalog.h"

// int main() {
//     initialize_catalog("angle_catalog.csv", "star_catalog.csv");
//     run_centroider("valid/raw-input-37.bin");

//     // for (int i = 0; i < MAX_ID_STARS; i++) {
//     //     image_star_t star = image_stars[i];
//     //     printf("Star at (%f, %f) with magnitude %f\n", star.camera.x, star.camera.y, star.magnitude);
//     // }

//     run_full_star_identifier(image_stars);

//     // for (int i = 0; i < MAX_ID_STARS; i++) {
//     //     float x = all_stars[i].image_star.camera.x;
//     //     float y = all_stars[i].image_star.camera.y;
//     //     uint32_t id = all_stars[i].catalog_star;

//     //     printf("{ { { %f, %f }, 0.0 }, %d },\n", x, y, id);
//     // }

//     float attitude[4] = {0.f};
//     run_attitude(all_stars, attitude);

//     //printf("Quaternion: [%f, %f, %f, %f]\n", attitude[0], attitude[1], attitude[2], attitude[3]);
// }   

// #include "attitude.h"
// #include "centroider.h"
// #include "star_identifier.h"
// #include "catalog.h"
// #include <stdio.h>
// #include <string.h>
// #include <time.h>
// #include <unistd.h>  // For access()

// #define MAX_IMAGES 1000

// int main() {
//     // Initialize the catalog once
//     initialize_catalog("angle_catalog.csv", "star_catalog.csv");

//     // Array to store valid image filenames
//     char image_filenames[MAX_IMAGES][256];
//     int valid_file_numbers[MAX_IMAGES];  // Store the corresponding file numbers
//     int valid_image_count = 0;

//     // Check for valid files and populate the array
//     for (int i = 0; i < MAX_IMAGES; i++) {
//         char filename[256];
//         snprintf(filename, sizeof(filename), "D:/image-test-float-big/raw-input-%d.bin", i);

//         if (access(filename, F_OK) == 0) {  // Check if the file exists
//             strncpy(image_filenames[valid_image_count], filename, sizeof(image_filenames[valid_image_count]));
//             valid_file_numbers[valid_image_count] = i;  // Store the file number
//             valid_image_count++;
//         }
//     }

//     // Start timing the processing of the images (wall-clock time)
//     time_t start_time = time(NULL);

//     // Process each valid image
//     for (int i = 0; i < valid_image_count; i++) {
    
//         // Get the current image filename
//         const char* image_filename = image_filenames[i];
//         int file_number = valid_file_numbers[i];
//         //printf("Processing file: raw-input-%d.bin ", file_number);  // Print the file number
        
//         printf("Iteration=%d\n", file_number);

//         // Process the image
//         run_centroider(image_filename);

//         // Run star identification and attitude estimation
//         run_full_star_identifier(image_stars);

//         // Calculate attitude
//         float attitude[4] = {0.f};
//         run_attitude(all_stars, attitude);
//     }

//     // Stop timing and calculate duration (wall-clock time)
//     time_t end_time = time(NULL);
//     double duration = difftime(end_time, start_time);  // Get the difference in seconds
//     printf("\nProcessed %d valid images in %f seconds.\n", valid_image_count, duration);

//     print_stats();

//     return 0;
// }

#include "attitude.h"
#include "centroider.h"
#include "star_identifier.h"
#include "catalog.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>  // For access()

#define MAX_IMAGES 10000

int main() {
    // Initialize the catalog once
    initialize_catalog("angle_catalog.csv", "star_catalog.csv");

    // Start timing the processing of the images (wall-clock time)
    time_t start_time = time(NULL);

    // Process each potential image file
    for (int i = 0; i < MAX_IMAGES; i++) {
        char filename[256];
        snprintf(filename, sizeof(filename), "D:/image-test-float-big/raw-input-%d.bin", i);

        if (access(filename, F_OK) == 0) {  // Check if the file exists
            printf("Processing file: raw-input-%d.bin (Iteration=%d)\n", i, i);

            // Process the image
            run_centroider(filename);

            // Run star identification and attitude estimation
            run_full_star_identifier(image_stars);

            // Calculate attitude
            float attitude[4] = {0.f};
            run_attitude(all_stars, attitude);
        }
    }

    // Stop timing and calculate duration (wall-clock time)
    time_t end_time = time(NULL);
    double duration = difftime(end_time, start_time);  // Get the difference in seconds
    printf("\nProcessed images in %f seconds.\n", duration);

    print_stats();

    return 0;
}


