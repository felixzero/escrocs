#include "us_control.h"
#include "../system/task_priority.h"
#include <esp_log.h>
#include <math.h>
#include <string.h>

#define TAG "US_controller"
//Don't forget to check the cone offset is same here and lua
#define CONE_OFFSET 2.513274122871834 //pi - pi/5

QueueHandle_t strategy_single_channel_queue, motion_cone_queue;

static bool EMPTY_CHANNELS[NUMBER_OF_US] = {false};

static TaskHandle_t task;
static bool active_channels[NUMBER_OF_US];
static int number_of_active_channels_cone = 0;

static void ultrasonic_board_task(void *parameters);
/**
 * Set the scanning action perimeter (angles in radian)
 * @return The number of active channels
 */
static int set_ultrasonic_scan_angle(float center_angle, float cone);

esp_err_t init_us_controller()
{
    // *** FreeRTOS "controller" Initialization ***
    strategy_single_channel_queue = xQueueCreate(1, sizeof(active_channels)); //Low priority Queue
    motion_cone_queue = xQueueCreate(1, sizeof(scan_angle_t)); //High priority Queue


    xTaskCreatePinnedToCore(
        ultrasonic_board_task,
        "ultrasonic_board",
        TASK_STACK_SIZE,
        NULL,
        US_BOARD_PRIORITY,
        &task,
        LOW_CRITICITY_CORE
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
    int8_t index_channel_to_scan = -1; //no channels to scan

    while (true) {
        //High priority scan
        if (xQueueReceive(motion_cone_queue, &motion_cone, 0) == pdTRUE) {
            ESP_LOGI(TAG, "motion_cone_queue dequeued");
            int active_channels = set_ultrasonic_scan_angle(motion_cone.center_angle + CONE_OFFSET, motion_cone.cone);
            ultrasonic_perform_scan();
            vTaskDelay(pdMS_TO_TICKS(ULTRASONIC_CHANNEL_PERIOD_MS) * active_channels);
            //set_ultrasonic_display_distances(motion_cone.center_angle - motion_cone.cone / 2, motion_cone.center_angle + motion_cone.cone / 2);
        }
        //Low priority scan
        else if (xQueueReceive(strategy_single_channel_queue, &channel_to_set, 0) == pdTRUE ) {
            index_channel_to_scan = 0;
            ESP_LOGI(TAG, "strategy_cone_queue dequeued");
        }
        //Make a low priority single channel scan
        else if(index_channel_to_scan >= 0) {
            //find next channel to scan
            while(channel_to_set[index_channel_to_scan] == false) { 
                index_channel_to_scan++;
                if(index_channel_to_scan >= NUMBER_OF_US) {
                    index_channel_to_scan = -1;
                    break;
                }
            }

            //scan next channel and block while doing it, in order to disable the scan just after
            bool channels[10] = {false};
            channels[index_channel_to_scan] = true;
            update_scan_channels(channels);
            vTaskDelay(pdMS_TO_TICKS(ULTRASONIC_CHANNEL_PERIOD_MS));
            update_scan_channels(EMPTY_CHANNELS);

        }
        else {
            //No scan to do, wait for a new one
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



