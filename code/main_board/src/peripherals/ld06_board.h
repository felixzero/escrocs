
#pragma once

#include <esp_err.h>
#include <stdbool.h>


#define LIDAR_I2C_ADDR 0x13
#define I2C_BUFFER_SIZE 64

#define I2C_REG_IS_OK 0x01
#define I2C_REG_CONE   0x03
#define I2C_REG_BOOL_OBSTACLE 0x10
#define I2C_REG_MM_OBSTACLE 0x11


esp_err_t init_ld06_board(void);

esp_err_t set_cone(float center_angle, float half_cone_width);
esp_err_t has_obstacle(bool *obstacle);
esp_err_t closest_obstacle(uint16_t *distance);
//esp_err_t read_all_distances(float *distances);