#include "attitude.h"

float quest_char_poly(float x, float a, float b, float c, float d, float s) {
    return (POW2(x) - a) * (POW2(x) - b) - (c * x) + (c * s) - d;
}

float quest_char_poly_prime(float x, float a, float b, float c) {
    return 4 * POW3(x) - 2 * (a + b) * x - c;
}

float quest_eigenvalue_estimator(float guess, float a, float b, float c, float d, float s, float epsilon) {
    float height;
    do {
        height = quest_char_poly(guess, a, b, c, d, s) / quest_char_poly_prime(guess, a, b, c);
        guess -= height;
    } while (fabs(height) >= epsilon);
    return guess;
}

void quaternion_to_euler(const float q[4], float *ra, float *de, float *roll) {
    float real = q[0];
    float i = q[1];
    float j = q[2];
    float k = q[3];

    *ra = atan2(2 * (-real * k + i * j), 1 - 2 * (j * j + k * k));
    if (*ra < 0) {
        *ra += 2 * M_PI;
    }
    *ra = *ra * (180.0 / M_PI);

    *de = -asin(2 * (-real * j - i * k));
    *de = *de * (180.0 / M_PI); 

    *roll = -atan2(2 * (-real * i + j * k), 1 - 2 * (i * i + j * j));
    if (*roll < 0) {
        *roll += 2 * M_PI;
    }
    *roll = *roll * (180.0 / M_PI);
}

void quest(identified_star_t identifed_stars[], float quaternion[], uint8_t n) {
    
    float B[9] = {0.f};
    float guess = 1;

    for (uint8_t i = 0; i < n; ++i) {
        coords_2d_t star_coords = identifed_stars[i].image_star.camera;
        uint32_t star_id = identifed_stars[i].catalog_star;
        float weight = 1.f / identifed_stars[i].image_star.magnitude;

        if (star_id == 0) {
            continue;
        }

        coords_3d_t cam_spatial_coords = {0.f};
        camera_to_spatial(star_coords, &cam_spatial_coords);
        float cam_spatial[3] = {cam_spatial_coords.x, cam_spatial_coords.y, cam_spatial_coords.z};

        catalog_entry_t star = star_list[star_id - 1];
        float cat_spatial[3] = {star.spatial.x, star.spatial.y, star.spatial.z};
        
        float outer[9] = {0.f};
        outer_product(cat_spatial, cam_spatial, outer, 3);
        scalar_multiply(outer, weight, outer, 9); 
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

    float eig = quest_eigenvalue_estimator(guess, a, b, c, d, sigma, EIGENVALUE_THRESHOLD);

    float alpha = (eig * eig) - (sigma * sigma) + kappa;
    float beta = eig - sigma;
    float gamma = (eig + sigma) * alpha - delta;

    float alpha_eye[9] = {alpha, 0.f, 0.f, 0.f, alpha, 0.f, 0.f, 0.f, alpha};
    
    float S_BETA[9] = {0.f};
    scalar_multiply(S, beta, S_BETA, 9);
    
    float X_temp[9] = {0.0f};
    add_three(alpha_eye, S_BETA, S_2, X_temp, 9);
    float X[3] = {0.0f};
    matrix_vector_multiply(X_temp, Z, X, 3);

    float scalar = 1 / (sqrt(gamma * gamma + dot(X, X, 3)));

    scalar_multiply(X, scalar, X, 3);
    gamma = gamma * scalar;

    quaternion[0] = gamma;
    quaternion[1] = X[0];
    quaternion[2] = X[1];
    quaternion[3] = X[2];
}

int assign_weights(identified_star_t identified_stars[]) {
    uint32_t count = 0;

    for (int i = 0; i < MAX_ID_STARS; i++) {
        identified_star_t star = identified_stars[i];
        uint32_t id = star.catalog_star;

        if (id != 0) {
            count += 1;
        }
    }

    for (int i = 0; i < MAX_ID_STARS; i++) {
        identified_stars[i].image_star.magnitude = count;
    }

    return count;
}

void run_attitude(identified_star_t identified_stars[], float attitude[]) {
    int count = assign_weights(identified_stars);
    if (count >= 4) {
        quest(identified_stars, attitude, MAX_ID_STARS);

        float ra, de, roll;
        quaternion_to_euler(attitude, &ra, &de, &roll);
        printf("RA=%f, Dec=%f, Roll=%f\n", ra, de, roll);
    }
}