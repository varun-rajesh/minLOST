#include "timer.h"

// Store the start time
static clock_t start_time;

static uint32_t discard_time = 0;
static uint32_t fail_id_time = 0;
static uint32_t id_time = 0;
static uint32_t centroid_time = 0;

// Starts the timer by recording the current time
void timer_start(void) {
    start_time = clock();
}

// Stops the timer and returns the elapsed time in milliseconds
uint32_t timer_stop(void) {
    if (start_time != 0) {
        clock_t end_time = clock();
        // Calculate the elapsed time in milliseconds
        uint32_t elapsed_time_ms = (end_time - start_time) * 1000 / CLOCKS_PER_SEC;

        start_time = 0;
        return elapsed_time_ms;
    } else {
        return 0;
    }
}

uint32_t timer_lap(void) {
    if (start_time != 0) {
        clock_t end_time = clock();
        // Calculate the elapsed time in milliseconds
        uint32_t elapsed_time_ms = (end_time - start_time) * 1000 / CLOCKS_PER_SEC;

        return elapsed_time_ms;
    } else {
        return 0;
    }
}

void timer_stop_discard(void) {
    discard_time += timer_stop();
}

void timer_stop_unid(void) {
    fail_id_time += timer_stop();
}

void timer_stop_id(void) {
    id_time += timer_stop();
}

void timer_centroid(void) {
    centroid_time += timer_lap();
}

void print_stats(void) {
    printf("Discard: %d, Fail: %d, ID: %d, Centroid: %d\n", discard_time, fail_id_time, id_time, centroid_time);
}