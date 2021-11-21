#pragma once

/**
 * Position of each channel, in degree
 */
typedef struct {
    float channel1;
    float channel2;
    float channel3;
} encoder_measurement_t;

/**
 * Read the current encoder positions
 */
void read_encoders(encoder_measurement_t *measurement);

/**
 * Set motor control voltages
 * @param speed in range [0, 1]
 */
void write_motor_speed(float speed1, float speed2, float speed3);
