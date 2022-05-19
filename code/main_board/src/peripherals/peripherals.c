#include "peripherals/peripherals.h"
#include "system/i2c_master.h"

#include <stdint.h>
#include <math.h>
#include <stdbool.h>

#include <esp_log.h>

#define TAG "Peripheral"
#define PUMP_I2C_ADDR 0x41
#define SERVO_I2C_ADDR 0x40
#define STEPPER1_I2C_ADDR 0x21
#define STEPPER2_I2C_ADDR 0x22

static uint8_t current_pump_reg = 0x00;
static uint8_t stepper_i2c_addr[] = { STEPPER1_I2C_ADDR, STEPPER2_I2C_ADDR };

static void write_servo_i2c_register(uint8_t reg, int pulse_width);

void init_peripherals(void)
{
    ESP_LOGI(TAG, "Initializing peripheral subsystem.");

    // Pump system init
    // Switch outputs off by default
    write_i2c_register(I2C_PORT_PERIPH, PUMP_I2C_ADDR, 0x01, current_pump_reg);
    // Define all canals as outputs
    write_i2c_register(I2C_PORT_PERIPH, PUMP_I2C_ADDR, 0x03, 0x00);

    // Servo system init
    // Disable low power mode and set register as auto-increment
    write_i2c_register(I2C_PORT_PERIPH, SERVO_I2C_ADDR, 0x00, 0x20);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    // Disable all outputs at startup
    write_i2c_register(I2C_PORT_PERIPH, SERVO_I2C_ADDR, 0xFA, 0x00);
    write_i2c_register(I2C_PORT_PERIPH, SERVO_I2C_ADDR, 0xFB, 0x00);
    write_servo_i2c_register(0xFA, 0);
    // Set PWM frequency to 50Hz (25MHz clock; 4096 max value)
    write_i2c_register(I2C_PORT_PERIPH, SERVO_I2C_ADDR, 0xFE, 121);
    // Totem pole output
    write_i2c_register(I2C_PORT_PERIPH, SERVO_I2C_ADDR, 0x01, 0x04);

    // Stepper motor system init
    for (size_t i = 0; i < 2; ++i) {
        // Set normal mode, listen only to own address
        write_i2c_register(I2C_PORT_PERIPH, stepper_i2c_addr[i], 0x00, 0x20);
        // Set two-phase drive
        write_i2c_register(I2C_PORT_PERIPH, stepper_i2c_addr[i], 0x0B, 0x50);
    }
}

void set_peripherals_pump(int channel, bool status)
{
    if (status) {
        ESP_LOGI(TAG, "Switching on pump %d", channel);
        current_pump_reg |= (1 << channel);
    } else {
        ESP_LOGI(TAG, "Switching off pump %d", channel);
        current_pump_reg &= ~(1 << channel);
    }
    write_i2c_register(I2C_PORT_PERIPH, PUMP_I2C_ADDR, 0x01, current_pump_reg);
}

void set_peripherals_servo_channel(int channel, int pulse_width)
{
    ESP_LOGI(TAG, "Setting target %d on servo channel %d", pulse_width, channel);
    write_servo_i2c_register(0x06 + 4 * channel, pulse_width);
}

static void write_servo_i2c_register(uint8_t reg, int pulse_width)
{
    uint16_t tick_width = pulse_width * 4096 / 20000;

    write_i2c_register(I2C_PORT_PERIPH, SERVO_I2C_ADDR, reg + 2, (uint8_t)tick_width);
    write_i2c_register(I2C_PORT_PERIPH, SERVO_I2C_ADDR, reg + 3, (uint8_t)(tick_width >> 8));
}

bool read_peripherals_motor_input(unsigned int motor_channel, unsigned int pin_channel)
{
    assert(motor_channel <= 1);
    assert(motor_channel <= 3);
    uint8_t reg = read_i2c_register(I2C_PORT_PERIPH, stepper_i2c_addr[motor_channel], 0x07);

    return reg & (1 << pin_channel);
}

void move_peripherals_motor(unsigned int motor_channel, int steps, float speed)
{
    assert(motor_channel <= 1);
    bool is_clockwise = steps >= 0;

    // Set number of steps
    uint16_t abs_steps = abs(steps);
    write_i2c_register(
        I2C_PORT_PERIPH,
        stepper_i2c_addr[motor_channel],
        is_clockwise ? 0x12 : 0x14,
        abs_steps & 0xFF
    );
    write_i2c_register(
        I2C_PORT_PERIPH,
        stepper_i2c_addr[motor_channel],
        is_clockwise ? 0x13 : 0x15,
        (abs_steps >> 8) & 0xFF
    );

    // Set speed (slowest prescaler)
    uint16_t raw_speed = fminf(128.0 / fabsf(speed), 0x1FFF);
    write_i2c_register(
        I2C_PORT_PERIPH,
        stepper_i2c_addr[motor_channel],
        is_clockwise ? 0x16 : 0x18,
        raw_speed & 0xFF
    );
    write_i2c_register(
        I2C_PORT_PERIPH,
        stepper_i2c_addr[motor_channel],
        is_clockwise ? 0x17 : 0x19,
        ((raw_speed >> 8) & 0xFF) | 0x40
    );

    // Set ramp up and down
    write_i2c_register(I2C_PORT_PERIPH, stepper_i2c_addr[motor_channel], 0x0D, 0x39);
    write_i2c_register(I2C_PORT_PERIPH, stepper_i2c_addr[motor_channel], 0x0E, 0x39);

    // Start motion
    write_i2c_register(I2C_PORT_PERIPH, stepper_i2c_addr[motor_channel], 0x1A, is_clockwise ? 0xC0 : 0xC1);
}
