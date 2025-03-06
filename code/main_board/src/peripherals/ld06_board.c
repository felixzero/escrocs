#include "ld06_board.h"
#include "../system/i2c_master.h"
#include "esp_log.h"
#include "math.h"

esp_err_t init_ld06_board(void) {
    uint8_t buffer[1] = {I2C_REG_IS_OK};
    ESP_LOGI("LD06", "sending lidar");
    send_to_i2c(I2C_PORT_PERIPH, LIDAR_I2C_ADDR, &buffer, 1);
    request_from_i2c(I2C_PORT_PERIPH, LIDAR_I2C_ADDR, &buffer, 1);
    ESP_LOGI("LD06", "bufer : %i", buffer[0]);
    return ESP_OK;
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
    *obstacle = buffer[0];
    return ESP_ERR_NOT_SUPPORTED; //TODO : check what it returns
}
esp_err_t closest_obstacle(float *distance) {
    uint8_t buffer[2] = {I2C_REG_MM_OBSTACLE, 0};
    send_to_i2c(I2C_PORT_PERIPH, LIDAR_I2C_ADDR, &buffer, 1);
    request_from_i2c(I2C_PORT_PERIPH, LIDAR_I2C_ADDR, &buffer, 2);
    *distance = buffer[0];
    return ESP_ERR_NOT_SUPPORTED; //TODO : check what it returns
}