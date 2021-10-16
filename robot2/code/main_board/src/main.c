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
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    calibrate_gyroscope_drift(10000 / portTICK_PERIOD_MS);
    zero_out_gyroscope();
    
    while(1) {
        ESP_LOGI("DEBUG", "Angle: %f", get_gyroscope_corrected_angle(0));
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
