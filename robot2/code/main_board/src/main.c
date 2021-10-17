#include "system/i2c_master.h"
#include "motion/motion_control.h"
#include "motion/gyroscope.h"
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

    init_gyroscope();

    while(1) {
        float yaw, pitch, roll;
        read_gyroscope(&yaw, &pitch, &roll);
        ESP_LOGI("DEBUG", "Yaw: %f", yaw);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    /*wheel_geometry_t geometry = {
        .wheel_radius = 28.5,
        .robot_radius = 122.26
    };
    init_motion_control(geometry);

    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }*/


}
