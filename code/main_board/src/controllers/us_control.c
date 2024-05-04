#include "us_control.h"
#include "../system/task_priority.h"
#include <esp_log.h>
#include <math.h>
#include <string.h>

#define TAG "US_controller"
// FIXME: Don't forget to check the cone offset is same here and lua
#define CONE_OFFSET 2.513274122871834 //pi - pi/5

QueueHandle_t strategy_single_channel_queue, motion_cone_queue, scan_over_queue;
uint16_t distances[NUMBER_OF_US] = { 0 };

static bool EMPTY_CHANNELS[NUMBER_OF_US] = { false };

static TaskHandle_t task;
static bool active_channels[NUMBER_OF_US];
static int number_of_active_channels_cone = 0;

static void ultrasonic_board_task(void *parameters);
static int set_ultrasonic_scan_angle(float center_angle, float cone);

esp_err_t init_us_controller()
{
    strategy_single_channel_queue = xQueueCreate(1, sizeof(active_channels)); // Low priority Queue
    motion_cone_queue = xQueueCreate(1, sizeof(scan_angle_t)); // High priority Queue
    scan_over_queue = xQueueCreate(1, sizeof(bool)); 

    xTaskCreatePinnedToCore(
        ultrasonic_board_task,
        "ultrasonic_board",
        TASK_STACK_SIZE,
        NULL,
        US_BOARD_PRIORITY,
        &task,
        TIME_CRITICAL_CORE
    );
    return ESP_OK;
}

int get_number_of_active_channels(void)
{
    return number_of_active_channels_cone;
}

static void ultrasonic_board_task(void *parameters)
{
    scan_angle_t motion_cone;
    bool channel_to_set[NUMBER_OF_US];

    while (true) {
        read_all_ultrasonic_values(distances); // Update distances
        // High priority scan
        if (xQueueReceive(motion_cone_queue, &motion_cone, 0) == pdTRUE) {
            int active_channels = set_ultrasonic_scan_angle(motion_cone.center_angle + CONE_OFFSET, motion_cone.cone);
            ultrasonic_perform_scan();
            vTaskDelay(pdMS_TO_TICKS(ULTRASONIC_CHANNEL_PERIOD_MS) * (active_channels + 1));
            bool scan_over = ultrasonic_has_obstacle();
            xQueueOverwrite(scan_over_queue, &scan_over);
            vTaskDelay(1);
        }
        // Low priority scan
        else if (xQueueReceive(strategy_single_channel_queue, &channel_to_set, 0) == pdTRUE) {
            int active_channels = 0;
            for(int i = 0; i < NUMBER_OF_US; i++) {
                if(channel_to_set[i]) {
                    active_channels++;
                }
            }
            update_scan_channels(channel_to_set);
            ultrasonic_perform_scan();
            vTaskDelay(pdMS_TO_TICKS(ULTRASONIC_CHANNEL_PERIOD_MS) * (active_channels + 1));
            update_scan_channels(EMPTY_CHANNELS);
            bool scan_over = ultrasonic_has_obstacle();
            xQueueOverwrite(scan_over_queue, &scan_over);
            vTaskDelay(1);
        }
        else {
            // No scan to do, wait for a new one
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

static int set_ultrasonic_scan_angle(float center_angle, float cone)
{
    bool channels_to_set[NUMBER_OF_US] = {false};
    for (int channel = 0; channel < NUMBER_OF_US; ++channel) {
        float channel_angle = 2 * M_PI * channel / NUMBER_OF_US;
        float channel_angle_offset = atan2(sin(channel_angle - center_angle), cos(channel_angle - center_angle));
        channels_to_set[NUMBER_OF_US - 1 - channel] = fabsf(channel_angle_offset) <= cone / 2;
    }

    if(memcmp(active_channels, channels_to_set, sizeof(active_channels)) != 0) {
        ESP_LOGI(TAG, "Setting new cone");
        update_scan_channels(channels_to_set); //enable channels in cone
        memcpy(active_channels, channels_to_set, sizeof(active_channels));
    }

    int number_of_active_channels = 0;
    for (int channel = 0; channel < NUMBER_OF_US; ++channel) {
        if (active_channels[channel]) {
            number_of_active_channels++;
        }
    }
    return number_of_active_channels;
}



