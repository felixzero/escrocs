#include "motion/motor_board.h"
#include "system/i2c_master.h"

#include <stdint.h>
#include <math.h>

#define MOTOR_I2C_ADDR 0x0A
#define TICK_PER_DEGREE 1.0

#define I2C_REG_MOTOR_PWM_0L    0x01
#define I2C_REG_ENCODER_0L      0x07

#define CLAMP_ABS(x, clamp) ((fabsf(x) > (clamp)) ? (clamp) * (x) / fabsf(x) : (x))

void read_encoders(encoder_measurement_t *measurement)
{
    uint8_t reg = I2C_REG_ENCODER_0L;
    int16_t encoder_raw_values[3];

    send_to_i2c(I2C_PORT_MOTOR, MOTOR_I2C_ADDR, &reg, 1);
    request_from_i2c(I2C_PORT_MOTOR, MOTOR_I2C_ADDR, encoder_raw_values, sizeof(encoder_raw_values));

    measurement->channel1 = (float)encoder_raw_values[0] / TICK_PER_DEGREE;
    measurement->channel2 = (float)encoder_raw_values[1] / TICK_PER_DEGREE;
    measurement->channel3 = (float)encoder_raw_values[2] / TICK_PER_DEGREE;
}

void write_motor_speed(float speed1, float speed2, float speed3)
{
    int8_t buffer[7];
    buffer[0] = I2C_REG_MOTOR_PWM_0L;
    *((int16_t*)&buffer[1]) = 255 * CLAMP_ABS(speed1, 1.0);
    *((int16_t*)&buffer[3]) = 255 * CLAMP_ABS(speed2, 1.0);
    *((int16_t*)&buffer[5]) = 255 * CLAMP_ABS(speed3, 1.0);
    send_to_i2c(I2C_PORT_MOTOR, MOTOR_I2C_ADDR, &buffer, sizeof(buffer));
}
