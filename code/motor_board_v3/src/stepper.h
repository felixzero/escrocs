#pragma once

#include <stdint.h>

#define NO_MOTION 65535
#define FASTEST_PERIOD_POSSIBLE 20

#define POLARITY_CW 1
#define POLARITY_CCW 0

struct __attribute__ ((__packed__)) acceleration_step {
    struct acceleration_step *next_step;
    uint16_t period_prefactor;
    uint16_t final_indice;
    uint16_t current_indice;
    int16_t indice_increment;
    uint8_t polarity;
};

extern struct acceleration_step stopped_step;

void init_stepper(void);

struct acceleration_step* build_acceleration_step(uint16_t initial_period, uint16_t final_period, uint16_t distance, uint8_t polarity);

void start_motion_immediately(struct acceleration_step* channel0, struct acceleration_step* channel1, struct acceleration_step* channel2);
void stop_motion_immediately(void);
