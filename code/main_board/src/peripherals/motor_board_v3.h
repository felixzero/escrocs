#pragma once

#include <esp_err.h>

/**
 * Position of each channel, in degree
 */
typedef struct {
    float channel1;
    float channel2;
    float channel3;
} encoder_measurement_t;

void init_motor_board_v3(void);

/**
 * Read the current encoder positions
 */
esp_err_t read_encoders(encoder_measurement_t *measurement);

/**
 * Set motor control voltages
 * @param speed in range [0, 1]
 */
esp_err_t write_motor_speed(float speed1, float speed2, float speed3);
