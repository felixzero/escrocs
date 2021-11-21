#include "motion/motor_board.h"
#include "system/i2c_master.h"

#include <stdint.h>
#include <math.h>

#define MOTOR_I2C_ADDR 0x0A
#define TICK_PER_DEGREE 1.0

#define CLAMP_ABS(x, clamp) ((fabs(x) > (clamp)) ? (clamp) * (x) / fabs(x) : (x))

void read_encoders(encoder_measurement_t *measurement)
{
    uint8_t reg = 0x02;
    int16_t encoder_raw_values[6];

    send_to_i2c(I2C_PORT_MOTOR, MOTOR_I2C_ADDR, &reg, 1);
    request_from_i2c(I2C_PORT_MOTOR, MOTOR_I2C_ADDR, encoder_raw_values, sizeof(encoder_raw_values));

    measurement->channel1 = (float)encoder_raw_values[0] / TICK_PER_DEGREE;
    measurement->channel2 = (float)encoder_raw_values[1] / TICK_PER_DEGREE;
    measurement->channel3 = (float)encoder_raw_values[2] / TICK_PER_DEGREE;
}

void write_motor_speed(float speed1, float speed2, float speed3)
{
    int8_t buffer[7];
    buffer[0] = 0x01;
    *((int16_t*)&buffer[1]) = 0.9 * CLAMP_ABS(speed1, 1.0) * INT16_MAX;
    *((int16_t*)&buffer[3]) = 0.9 * CLAMP_ABS(speed2, 1.0) * INT16_MAX;
    *((int16_t*)&buffer[5]) = 0.9 * CLAMP_ABS(speed3, 1.0) * INT16_MAX;
    send_to_i2c(I2C_PORT_MOTOR, MOTOR_I2C_ADDR, &buffer, sizeof(buffer));
}
