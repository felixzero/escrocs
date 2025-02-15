#include "peripherals/motor_board.h"
#include "system/i2c_master.h"

#define __THIS_VERSION 2
#if MOTOR_BOARD_VERSION == __THIS_VERSION
#include <stdint.h>
#include <math.h>

#include "esp_log.h"

#define MOTOR_I2C_ADDR 0x0A
#define TICK_PER_DEGREE 1.0

#define I2C_REG_MOTOR_PWM_0L    0x01
#define I2C_REG_ENCODER_0L      0x07

#define CLAMP_ABS(x, clamp) ((fabsf(x) > (clamp)) ? (clamp) * (x) / fabsf(x) : (x))

esp_err_t init_motor_board(void) {
    encoder_measurement_t measurement;
    if (read_encoder_increment(&measurement) == ESP_OK) {
        return ESP_OK;
    }
    return ESP_ERR_NOT_FOUND;
}
/**
 * Read the current encoder positions
 */
esp_err_t read_encoder_increment(encoder_measurement_t *measurement) {
    uint8_t reg = I2C_REG_ENCODER_0L;
    int16_t encoder_raw_values[3];

    send_to_i2c(I2C_PORT_MOTOR, MOTOR_I2C_ADDR, &reg, 1);
    request_from_i2c(I2C_PORT_MOTOR, MOTOR_I2C_ADDR, encoder_raw_values, sizeof(encoder_raw_values));

    measurement->channel1 = (float)encoder_raw_values[0] / TICK_PER_DEGREE;
    measurement->channel2 = (float)encoder_raw_values[1] / TICK_PER_DEGREE;
    measurement->channel3 = (float)encoder_raw_values[2] / TICK_PER_DEGREE;
    return ESP_OK;
}

/**
 * @param speed in range [0, 1] or in RPM
 */
esp_err_t write_motor_speed_raw(float speed1, float speed2, float speed3) {
    int8_t buffer[7];
    buffer[0] = I2C_REG_MOTOR_PWM_0L;
    *((int16_t*)&buffer[1]) = 160 * CLAMP_ABS(speed1, 1.0); //160 instead of 255 due to 18V battery
    *((int16_t*)&buffer[3]) = 160 * CLAMP_ABS(speed2, 1.0);
    *((int16_t*)&buffer[5]) = 160 * CLAMP_ABS(speed3, 1.0);
    send_to_i2c(I2C_PORT_MOTOR, MOTOR_I2C_ADDR, &buffer, sizeof(buffer));
    return ESP_OK;
}
esp_err_t write_motor_speed_rad_s(float speed1, float speed2, float speed3) {
    return ESP_OK;
}

/**
 * Enable or disable control of the stepper motors.
 * This can be used to save battery during long idling times.
 */
esp_err_t enable_motors(void) {
    return ESP_OK;
}
esp_err_t disable_motors(void) {
    return ESP_OK;
}

bool are_motors_enabled(void) {
    return true;
}

#endif