#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

// Define the I2C slave address
#define I2C_SLAVE_ADDRESS 0x12 //Same as US board

// Function prototypes
void i2c_slave_task(void *pvParameters);
void i2c_slave_write(uint8_t *data, size_t length);
void i2c_slave_read(uint8_t *data, size_t length);

#endif // I2C_SLAVE_H
 