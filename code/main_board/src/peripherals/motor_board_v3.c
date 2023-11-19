#include "motor_board_v3.h"
#include "../system/modbus_rtu_master.h"

#include <esp_log.h>
#include <math.h>

#define MOTOR_BOARD_MODBUS_ADDR     0x44

#define MOTOR_BOARD_SPEED_REG       10000
#define MOTOR_BOARD_ENCODER_REG     10100
#define MOTOR_BOARD_ENABLE_COIL     20000

#define TICK_PER_TURN               200.0
#define TICK_PER_DEGREE             (TICK_PER_TURN / 360.0)
#define MAX_ID_LEN                  64
#define TAG                         "Motor board v3"

#define CLAMP_ABS(x, clamp) ((fabsf(x) > (clamp)) ? (clamp) * (x) / fabsf(x) : (x))

esp_err_t init_motor_board_v3(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing motor board subsystem.");
    static char id[MAX_ID_LEN] = "";
    err = modbus_read_device_identification(MOTOR_BOARD_MODBUS_ADDR, 0x00, id, MAX_ID_LEN - 1);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }
    ESP_LOGI(TAG, "Vendor name: %s", id);


    err = modbus_read_device_identification(MOTOR_BOARD_MODBUS_ADDR, 0x01, id, MAX_ID_LEN - 1);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }
    ESP_LOGI(TAG, "Product code: %s", id);

    err = modbus_read_device_identification(MOTOR_BOARD_MODBUS_ADDR, 0x02, id, MAX_ID_LEN - 1);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }
    ESP_LOGI(TAG, "Revision: %s", id);

    return ESP_OK;
}

esp_err_t read_encoders(encoder_measurement_t *measurement)
{
    int16_t encoder_raw_values[3];
    esp_err_t err = modbus_read_holding_registers(MOTOR_BOARD_MODBUS_ADDR, MOTOR_BOARD_ENCODER_REG, 3, (uint16_t*)encoder_raw_values);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);

    if (err == ESP_OK) {
        measurement->channel1 = (float)encoder_raw_values[0] / TICK_PER_DEGREE;
        measurement->channel2 = (float)encoder_raw_values[1] / TICK_PER_DEGREE;
        measurement->channel3 = (float)encoder_raw_values[2] / TICK_PER_DEGREE;
    }

    return err;
}

esp_err_t write_motor_speed_raw(float speed1, float speed2, float speed3)
{
    int16_t raw_values[3];
    raw_values[0] = INT16_MAX * CLAMP_ABS(speed1, 1.0);
    raw_values[1] = INT16_MAX * CLAMP_ABS(speed2, 1.0);
    raw_values[2] = INT16_MAX * CLAMP_ABS(speed3, 1.0);

    esp_err_t err = modbus_preset_multiple_registers(MOTOR_BOARD_MODBUS_ADDR, MOTOR_BOARD_SPEED_REG, 3, (uint16_t*)raw_values);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);

    return err;
}

esp_err_t write_motor_speed_rpm(float speed1, float speed2, float speed3)
{
    const float MAX_SPEED_RPM = 300;
    return write_motor_speed_raw(speed1 / MAX_SPEED_RPM, speed2 / MAX_SPEED_RPM, speed3 / MAX_SPEED_RPM);
}

esp_err_t enable_motors(void)
{
    esp_err_t err = modbus_force_single_coil(MOTOR_BOARD_MODBUS_ADDR, MOTOR_BOARD_ENABLE_COIL, true);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);

    return err;
}

esp_err_t disable_motors(void)
{
    esp_err_t err = modbus_force_single_coil(MOTOR_BOARD_MODBUS_ADDR, MOTOR_BOARD_ENABLE_COIL, false);
    ESP_ERROR_CHECK_WITHOUT_ABORT(err);

    return err;
}

bool are_motors_enabled(void)
{
    bool output;
    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_read_coil_status(MOTOR_BOARD_MODBUS_ADDR, MOTOR_BOARD_ENABLE_COIL, 1, &output));
    return output;
}