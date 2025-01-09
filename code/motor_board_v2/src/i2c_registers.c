#include "i2c_registers.h"
#include "encoder.h"
#include "motor_pwm.h"

#include <avr/io.h>

#define INFO_VALUE 0x99
#define BAD_REG_VALUE 0xFF

static int16_t motor_pwm_0 = 0, motor_pwm_1 = 0, motor_pwm_2 = 0;
static void update_status_led(void);

uint8_t read_i2c_register(uint8_t reg)
{
    switch (reg) {
    case I2C_REG_INFO:
        return INFO_VALUE;
    case I2C_REG_MOTOR_PWM_0L:
        return *(uint8_t*)(&motor_pwm_0);
    case I2C_REG_MOTOR_PWM_0H:
        return *(uint8_t*)(&motor_pwm_0 + 1);
    case I2C_REG_MOTOR_PWM_1L:
        return *(uint8_t*)(&motor_pwm_1);
    case I2C_REG_MOTOR_PWM_1H:
        return *(uint8_t*)(&motor_pwm_1 + 1);
    case I2C_REG_MOTOR_PWM_2L:
        return *(uint8_t*)(&motor_pwm_2);
    case I2C_REG_MOTOR_PWM_2H:
        return *(uint8_t*)(&motor_pwm_2 + 1);
    case I2C_REG_ENCODER_0L:
        return *((uint8_t*)&encoder_values[0]);
    case I2C_REG_ENCODER_0H:
        return *((uint8_t*)&encoder_values[0] + 1);
    case I2C_REG_ENCODER_1L:
        return *((uint8_t*)&encoder_values[1]);
    case I2C_REG_ENCODER_1H:
        return *((uint8_t*)&encoder_values[1] + 1);
    case I2C_REG_ENCODER_2L:
        return *((uint8_t*)&encoder_values[2]);
    case I2C_REG_ENCODER_2H:
        return *((uint8_t*)&encoder_values[2] + 1);
    default:
        return BAD_REG_VALUE;
    }
}

void write_i2c_register(uint8_t reg, uint8_t value)
{
    switch (reg) {
    case I2C_REG_MOTOR_PWM_0L:
        *((uint8_t*)&motor_pwm_0) = value;
        return;
    case I2C_REG_MOTOR_PWM_0H:
        *((uint8_t*)&motor_pwm_0 + 1) = value;
        write_motor_channel(0, motor_pwm_0);
        update_status_led();
        return;
    case I2C_REG_MOTOR_PWM_1L:
        *((uint8_t*)&motor_pwm_1) = value;
        return;
    case I2C_REG_MOTOR_PWM_1H:
        *((uint8_t*)&motor_pwm_1 + 1) = value;
        write_motor_channel(1, motor_pwm_1);
        update_status_led();
        return;
    case I2C_REG_MOTOR_PWM_2L:
        *((uint8_t*)&motor_pwm_2) = value;
        return;
    case I2C_REG_MOTOR_PWM_2H:
        *((uint8_t*)&motor_pwm_2 + 1) = value;
        write_motor_channel(2, motor_pwm_2);
        update_status_led();
        return;
    default:
        return;
    }
}

static void update_status_led(void)
{
    if (motor_pwm_0 != 0 || motor_pwm_1 != 0 || motor_pwm_2 != 0) {
        PORTB |= _BV(0);
    } else {
        PORTB &= ~_BV(0);
    }
}