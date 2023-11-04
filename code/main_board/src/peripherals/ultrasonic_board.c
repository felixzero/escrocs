#include "peripherals/ultrasonic_board.h"
#include "system/modbus_rtu_master.h"

#include <esp_log.h>
#include <esp_err.h>
#include <math.h>

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

void init_ultrasonic_board(void)
{
    ESP_LOGI(TAG, "Initializing ultrasonic detection subsystem.");
    static char id[MAX_ID_LEN] = "";
    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_read_device_identification(MODBUS_ADDRESS, 0x00, id, MAX_ID_LEN - 1));
    ESP_LOGI(TAG, "Vendor name: %s", id);
    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_read_device_identification(MODBUS_ADDRESS, 0x01, id, MAX_ID_LEN - 1));
    ESP_LOGI(TAG, "Product code: %s", id);
    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_read_device_identification(MODBUS_ADDRESS, 0x02, id, MAX_ID_LEN - 1));
    ESP_LOGI(TAG, "Revision: %s", id);

    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_preset_single_register(MODBUS_ADDRESS, MODBUS_CONFIG_CRITICAL_DISTANCE, CRITICAL_DISTANCE_MM));
    disable_ultrasonic_detection();
}

void set_ultrasonic_scan_angle(float min_angle, float max_angle)
{
    bool active_channels[NUMBER_OF_US];
    for (int channel = 0; channel < NUMBER_OF_US; ++channel) {
        float channel_angle = 2 * M_PI * channel / NUMBER_OF_US;
        active_channels[channel] = (channel_angle >= min_angle) && (channel_angle <= max_angle);
    }

    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_force_multiple_coils(MODBUS_ADDRESS, MODBUS_COIL_CHANNELS_START, NUMBER_OF_US, active_channels));
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

void disable_ultrasonic_detection(void)
{
    bool inputs[NUMBER_OF_US] = { false };
    ESP_ERROR_CHECK_WITHOUT_ABORT(modbus_force_multiple_coils(MODBUS_ADDRESS, MODBUS_COIL_CHANNELS_START, NUMBER_OF_US, inputs));
}
