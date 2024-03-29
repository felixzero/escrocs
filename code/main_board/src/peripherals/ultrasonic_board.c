#include "peripherals/ultrasonic_board.h"
#include "system/modbus_rtu_master.h"

#include <esp_log.h>
#include <esp_err.h>
#include <math.h>
#include <string.h>

#define TAG                             "Ultrasonic board"
#define MAX_ID_LEN                      64
#define NUMBER_OF_US                    10

#define CRITICAL_DISTANCE_MM            400

#define MODBUS_ADDRESS                  0x45

#define MODBUS_CONFIG_CRITICAL_DISTANCE 0010
#define MODBUS_CONFIG_SAFE_DISTANCE     0011
#define MODBUS_CONFIG_REPETITION_PERIOD 0020
#define MODBUS_CONFIG_TIMEOUT           0021
#define MODBUS_COIL_CHANNELS_START      1000
#define MODBUS_DISTANCE_INPUT_REG_START 2000
#define MODBUS_COIL_PULSE               3000
#define MODBUS_COIL_UPDATE_LED          3001
#define MODBUS_COIL_PULSE_AND_UPDATE    3002
#define MODBUS_OBSTRUCTED_STATUS        4000

esp_err_t init_ultrasonic_board(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing ultrasonic detection subsystem.");
    static char id[MAX_ID_LEN] = "";
    err = modbus_read_device_identification(MODBUS_ADDRESS, 0x00, id, MAX_ID_LEN - 1);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }
    ESP_LOGI(TAG, "Vendor name: %s", id);

    err = modbus_read_device_identification(MODBUS_ADDRESS, 0x01, id, MAX_ID_LEN - 1);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }
    ESP_LOGI(TAG, "Product code: %s", id);

    err = modbus_read_device_identification(MODBUS_ADDRESS, 0x02, id, MAX_ID_LEN - 1);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }
    ESP_LOGI(TAG, "Revision: %s", id);

    err = modbus_preset_single_register(MODBUS_ADDRESS, MODBUS_CONFIG_CRITICAL_DISTANCE, CRITICAL_DISTANCE_MM);
    if (err) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(err);
        return err;
    }
    disable_all_ultrasonic_detection();

    return ESP_OK;
}

int set_ultrasonic_scan_angle(float center_angle, float cone)
{
    bool active_channels[NUMBER_OF_US];
    for (int channel = 0; channel < NUMBER_OF_US; ++channel) {
        float channel_angle = 2 * M_PI * channel / NUMBER_OF_US;
        float channel_angle_offset = atan2(sin(channel_angle - center_angle), cos(channel_angle - center_angle));
        active_channels[NUMBER_OF_US - 1 - channel] = fabsf(channel_angle_offset) <= cone / 2;
    }

    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_force_multiple_coils(MODBUS_ADDRESS, MODBUS_COIL_CHANNELS_START, NUMBER_OF_US, active_channels));

    int number_of_active_channels = 0;
    for (int channel = 0; channel < NUMBER_OF_US; ++channel) {
        if (active_channels[channel]) {
            number_of_active_channels++;
        }
    }
    return number_of_active_channels;
}

void set_ultrasonic_display_distances(float critical_distance, float safe_distance)
{
    uint16_t values[] = { roundf(critical_distance), roundf(safe_distance) };
    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_preset_multiple_registers(MODBUS_ADDRESS, MODBUS_CONFIG_CRITICAL_DISTANCE, 2, values));
}

void set_ultrasonic_repetition_period(int period_ms)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_preset_single_register(MODBUS_ADDRESS, MODBUS_CONFIG_REPETITION_PERIOD, period_ms));
}

void set_ultrasonic_timeout_distance(float distance_mm)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_preset_single_register(MODBUS_ADDRESS, MODBUS_CONFIG_TIMEOUT, roundf(distance_mm)));
}

void ultrasonic_perform_scan(void)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_force_single_coil(MODBUS_ADDRESS, MODBUS_COIL_PULSE_AND_UPDATE, true));
}

bool ultrasonic_has_obstacle(void)
{
    bool result = false;
    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_read_input_status(MODBUS_ADDRESS, MODBUS_OBSTRUCTED_STATUS, 1, &result));
    return result;
}

int disable_all_ultrasonic_detection(void)
{
    bool inputs[NUMBER_OF_US] = { false };
    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_force_multiple_coils(MODBUS_ADDRESS, MODBUS_COIL_CHANNELS_START, NUMBER_OF_US, inputs));
    return 0;
}

int enable_all_ultrasonic_detection(void)
{
    bool inputs[NUMBER_OF_US];
    memset(inputs, true, NUMBER_OF_US);
    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_force_multiple_coils(MODBUS_ADDRESS, MODBUS_COIL_CHANNELS_START, NUMBER_OF_US, inputs));
    return NUMBER_OF_US;
}
