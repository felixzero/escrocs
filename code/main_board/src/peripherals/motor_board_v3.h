#pragma once

#include <esp_err.h>
#include <stdbool.h>

/**
 * Position of each channel, in degree
 */
typedef struct {
    float channel1;
    float channel2;
    float channel3;
} encoder_measurement_t;

esp_err_t init_motor_board_v3(void);

/**
 * Read the current encoder positions
 */
esp_err_t read_encoder_increment(encoder_measurement_t *measurement);

/**
 * Set motor control voltages
 * @param speed in range [0, 1] or in RPM
 */
esp_err_t write_motor_speed_raw(float speed1, float speed2, float speed3);
esp_err_t write_motor_speed_rpm(float speed1, float speed2, float speed3);

/**
 * Enable or disable control of the stepper motors.
 * This can be used to save battery during long idling times.
 */
esp_err_t enable_motors(void);
esp_err_t disable_motors(void);
bool are_motors_enabled(void);
