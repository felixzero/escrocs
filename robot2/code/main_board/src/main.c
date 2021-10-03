#include "system/i2c_master.h"
#include "motion/motion_control.h"
#include "wireless/wifi.h"
#include "wireless/httpd.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

void app_main() {
    ESP_LOGI("main", "Starting ESCRObot application...\n");
    init_i2c_master();
    init_wifi_system();
    init_http_server();

    feedback_params_t pid = { 0.006, 0.00002, 0.0 };
    wheel_geometry_t geometry = {
        .wheel_radius = 28.5,
        .robot_radius = 122.26,
        .friction_coefficient = 0.0,
        .max_speed = 0.9
    };
    init_motion_control(pid, geometry);

/*int i = 0;
    while (1) {
        if (is_motion_done()) {
            if (i % 2 == 0) {
                set_motion_target(500.0, 0.0, 0.0);
            } else {
                set_motion_target(0.0, 0.0, 0.0);
            }
            i++;
        }
        motion_control_loop();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }*/

    /*while (1) {
    for(float x = 0; x < 500.0; x += 20) {
        set_motion_target(x, 0, 0);
        motion_control_loop();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    for(float x = 500; x > 0; x -= 20) {
        set_motion_target(x, 0, 0);
        motion_control_loop();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    }*/

    while(1) {
    for(float x = 0; x < 500.0; x += 20) {
        set_motion_target(x, 0, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    }
}
