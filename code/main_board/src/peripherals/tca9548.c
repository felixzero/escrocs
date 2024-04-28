#include "tca9548.h"

esp_err_t tca9548_select_channel(uint8_t channel) {
    
    write_i2c_register(I2C_PORT_TCA9548, TCA9548_ADDR_0, (1 << channel), 1);
    return ESP_OK;
}