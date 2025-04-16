#include "avoidance_control.h"

#ifdef LIDAR_CONTROL
#include "../system/task_priority.h"
#include "../peripherals/ld06_board.h"
#include <esp_log.h>
#include <math.h>
#include <string.h>

#define TAG "lidar_control"

#define CONE_OFFSET 0 

#define US_OBSTACLE_DISTANCE_MM     450
#define US_WARNING_DISTANCE_MM      (2 * US_OBSTACLE_DISTANCE_MM)

QueueHandle_t strategy_single_channel_queue, motion_cone_queue, scan_over_queue;
uint16_t distances[NUMBER_OF_US]; //Uselesss here but for US compatiblity
static TaskHandle_t task;

static void lidar_board_task(void *parameters);
static int set_lidar_scan_angle(float center_angle, float cone);

esp_err_t init_mockup() { //To use when the lidar is not really connected
    motion_cone_queue = xQueueCreate(1, sizeof(scan_angle_t)); // High priority Queue
    scan_over_queue = xQueueCreate(1, sizeof(bool));
    return ESP_OK;
}
esp_err_t init_avoidance_controller()
{
    motion_cone_queue = xQueueCreate(1, sizeof(scan_angle_t)); // High priority Queue
    scan_over_queue = xQueueCreate(1, sizeof(bool));

    //set_ultrasonic_display_distances(US_OBSTACLE_DISTANCE_MM, US_WARNING_DISTANCE_MM);

    xTaskCreatePinnedToCore(
        lidar_board_task,
        "lidar_board_task",
        TASK_STACK_SIZE,
        NULL,
        US_BOARD_PRIORITY,
        &task,
        TIME_CRITICAL_CORE
    );
    return ESP_OK;
}

static void lidar_board_task(void *parameters)
{
    scan_angle_t motion_cone;
    bool scan_over;
    uint16_t dist = 0, last_dist = 0;
    float last_center = 0, last_cone = 0;
    uint8_t iteration_since_last = 0;
    while (true) {
        if (xQueueReceive(motion_cone_queue, &motion_cone, 0) == pdTRUE) {
            if(motion_cone.center_angle != last_center || motion_cone.cone != last_cone) {
                last_center = motion_cone.center_angle;
                last_cone = motion_cone.cone;
                set_cone(motion_cone.center_angle, motion_cone.cone);
                ESP_LOGI(TAG, "set cone to %f %f", motion_cone.center_angle, motion_cone.cone);
            }
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
        ESP_ERROR_CHECK_WITHOUT_ABORT(has_obstacle(&scan_over));
        ESP_ERROR_CHECK_WITHOUT_ABORT(closest_obstacle(&dist));
        if(dist != last_dist || iteration_since_last > 2) {
            iteration_since_last = 0;
            last_dist = dist;
            xQueueOverwrite(scan_over_queue, &scan_over);
            ESP_LOGI(TAG, "closest obstacle %i", dist);
        } 
        else {
            iteration_since_last++;
        }
        //ESP_LOGI(TAG, "scan over %i", scan_over);

    }
}


esp_err_t set_lidar_distance(uint16_t obstacle_distance) {// Unimplemented ->, uint16_t warning_distance)
    return ESP_ERR_NOT_SUPPORTED;
}

#endif