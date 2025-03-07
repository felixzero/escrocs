#pragma once

#define I2C_SLAVE_ADDRESS 0x13 //Same as US board
#define I2C_BUFFER_SIZE 64

#define I2C_REG_IS_OK 0x01
#define I2C_REG_CONE   0x03
#define I2C_REG_BOOL_OBSTACLE 0x10
#define I2C_REG_MM_OBSTACLE 0x11
// Function prototypes
void i2c_slave_task(void *pvParameters);
 