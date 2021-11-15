#include "peripherals/peripherals.h"
#include "system/i2c_master.h"

#include <stdint.h>

#include <esp_log.h>

#define TAG "Peripheral"
#define PUMP_I2C_ADDR 0x41
#define SERVO_I2C_ADDR 0x40
#define STEPPER1_I2C_ADDR 0x22
#define STEPPER2_I2C_ADDR 0x21

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
    ESP_LOGI(TAG, "%x", read_i2c_register(I2C_PORT_PERIPH, STEPPER1_I2C_ADDR, 0x00));
    ESP_LOGI(TAG, "%x", read_i2c_register(I2C_PORT_PERIPH, STEPPER2_I2C_ADDR, 0x00));
}

void set_peripherals_pump(int channel, bool status)
{
    if (status) {
        current_pump_reg |= (1 << channel);
    } else {
        current_pump_reg &= ~(1 << channel);
    }
    write_i2c_register(I2C_PORT_PERIPH, PUMP_I2C_ADDR, 0x01, current_pump_reg);
}

void set_peripherals_servo_channel(int channel, int pulse_width)
{
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
