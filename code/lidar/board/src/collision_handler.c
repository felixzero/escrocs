#include "collision_handler.h"
#include "math.h"
#include "lidar.h"
#include "stdio.h"
#include "stdint.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

//Convert angle to the range [-pi; +pi]
#define CENTIDEGTORAD(x) (x < 18000) ? (x * 0.000174533f) : (x * 0.000174533f - (2 * M_PI))
static float normalize_angle(float angle);
static bool is_within_cone(float angle_rad, float cone_center_rad, float cone_half_width_rad);
static float cone_half_width_rad = 0.1f;
static float cone_center_rad = 0.0f; 
static uint16_t obs_trig_dist_min = OBSTACLE_TRIG_DIST_MIN_MM;
static uint16_t obs_trig_dist_stop = OBSTACLE_TRIG_DIST_STOP_MM;

polar_t polar_array[MAX_AMALG_COUNT];
uint16_t nb_cur_amalg = 0;


void update_amalgames_task(void *pvParameters) {
    polar_array_t temp_polar;
    while(1) {
        if (xQueueReceive(polar_array_queue, &temp_polar, 0) == pdTRUE) {
            for (int i = 0; i < temp_polar.length; i++) {
                if(temp_polar.pts[i].distance == 0 && temp_polar.pts[i].angle == 0) {
                    nb_cur_amalg = i;
                    break;
                }
                polar_array[i] = temp_polar.pts[i];
            }
        } else {
            // No data in the queue, perform other tasks or sleep briefly
            vTaskDelay(pdMS_TO_TICKS(10)); // Sleep for 10 milliseconds
        }
    }
}

bool has_obstacle() {
    for (size_t i = 0; i < nb_cur_amalg; i++)
    {
        if (polar_array[i].distance > obs_trig_dist_min && polar_array[i].distance < obs_trig_dist_stop
        && is_within_cone(CENTIDEGTORAD(polar_array[i].angle), cone_center_rad, cone_half_width_rad)) {
            return true;
        }
    }
    return false;
}

uint16_t closest_obstacle_dist() {
    uint16_t min_dist = 0xFFFF;
    uint16_t angle = 0;
    for (size_t i = 0; i < nb_cur_amalg; i++)
    {
        //ESP_LOGI("col_han", "angle %f, dist %i", CENTIDEGTORAD(polar_array[i].angle), polar_array[i].distance);
        if (polar_array[i].distance > obs_trig_dist_min
        && is_within_cone(CENTIDEGTORAD(polar_array[i].angle), cone_center_rad, cone_half_width_rad)) {
            if(polar_array[i].distance < min_dist) {
                min_dist = polar_array[i].distance;
                angle = polar_array[i].angle;
            }
        }
    }
    //ESP_LOGI("COLL TEST", "angle %i, distance %i", angle, min_dist);
    return min_dist;
}
esp_err_t update_cone(float center_cone, float half_width_cone) {
    cone_center_rad = center_cone;
    cone_half_width_rad = half_width_cone;
    return ESP_OK;
}
esp_err_t update_dist(uint16_t new_obstacle_trig_dist) {
    obs_trig_dist_stop = new_obstacle_trig_dist;
    return ESP_OK;
}

static float normalize_angle(float angle) {
    while (angle < 0) {
        angle += 2 * M_PI;
    }
    while (angle > 2 * M_PI) {
        angle -= 2 * M_PI;
    }
    return angle;
}

static bool is_within_cone(float angle_rad, float cone_center_rad, float cone_half_width_rad) {
    angle_rad = normalize_angle(angle_rad);
    cone_center_rad = normalize_angle(cone_center_rad);
    float diff = angle_rad - cone_center_rad;
    diff = normalize_angle(diff + M_PI) - M_PI;
    return fabsf(diff) < cone_half_width_rad;
}