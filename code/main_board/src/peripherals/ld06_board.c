#include "ld06_board.h"
#include "../system/i2c_master.h"
#include "esp_log.h"
#include "math.h"

esp_err_t init_ld06_board(void) {
    uint8_t buffer[1] = {I2C_REG_IS_OK};
    send_to_i2c(I2C_PORT_PERIPH, LIDAR_I2C_ADDR, &buffer, 1);
    buffer[0] = 3;
    request_from_i2c(I2C_PORT_PERIPH, LIDAR_I2C_ADDR, &buffer, 1);
    switch (buffer[0])
    {
    case 0x03:
        return ESP_ERR_NOT_FOUND; //The PCB is not connected
    case 0x04:
        return ESP_ERR_NOT_FINISHED; //The PCB is connected but the LD06 is not ready
    case 0x05:
       return ESP_OK; //The PCB is connected and the LD06 is ready
    default:
        return ESP_ERR_INVALID_RESPONSE; //Shouldn't receive it from the ld06 board
        break;
    }
}

esp_err_t set_cone(float center_angle, float half_cone_width) {
    uint8_t center_angle_mapped = (uint8_t)((center_angle + M_PI) / (2 * M_PI) * 256);
    uint8_t half_cone_width_mapped = (uint8_t)(half_cone_width / M_PI * 256);
    uint8_t center[3] = {I2C_REG_CONE, center_angle_mapped, half_cone_width_mapped};
    send_to_i2c(I2C_PORT_PERIPH, LIDAR_I2C_ADDR, &center, 3);
    return ESP_OK;
}
esp_err_t has_obstacle(bool *obstacle) {
    uint8_t buffer[1] = {I2C_REG_BOOL_OBSTACLE};
    send_to_i2c(I2C_PORT_PERIPH, LIDAR_I2C_ADDR, &buffer, 1);
    request_from_i2c(I2C_PORT_PERIPH, LIDAR_I2C_ADDR, &buffer, 1);
    *obstacle = (buffer[0] == 5) ? true : false; //O and 1 seems to not work properly
    if(buffer[0] != 4 && buffer[0] != 5) {
        return ESP_ERR_INVALID_RESPONSE;
    }
    return ESP_OK;
}
esp_err_t closest_obstacle(uint16_t *distance) {
    uint8_t buffer[2] = {I2C_REG_MM_OBSTACLE, 0};
    send_to_i2c(I2C_PORT_PERIPH, LIDAR_I2C_ADDR, &buffer, 1);
    request_from_i2c(I2C_PORT_PERIPH, LIDAR_I2C_ADDR, &buffer, 2);
    *distance = buffer[0] | buffer[1] << 8;
    if(buffer[0] == 0 && buffer[1] == 0) {
        return ESP_ERR_INVALID_RESPONSE;
    }
    return ESP_OK;
}