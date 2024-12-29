#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

// Starts the timer
void timer_start(void);

// Stops the timer and returns the elapsed time in milliseconds
uint32_t timer_stop(void);

uint32_t timer_lap(void);

void timer_stop_discard(void);

void timer_stop_unid(void);

void timer_stop_id(void);

void timer_centroid(void);

void print_stats(void);

#endif // TIMER_H
