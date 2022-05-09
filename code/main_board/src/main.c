#include "system/i2c_master.h"
#include "system/spiffs.h"
#include "wireless/wifi.h"
#include "wireless/httpd.h"
#include "peripherals/peripherals.h"
#include "peripherals/stepper_board.h"
#include "motion/motion_control.h"
#include "actions/strategy.h"
#include "motion/motor_board.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>


#include "peripherals/lidar_board.h"
void app_main() {
    ESP_LOGI("main", "Starting ESCRObot application...\n");
    init_i2c_master();
    init_spiffs();
    init_wifi_system();
    init_http_server();
    vTaskDelay(200);
    //init_peripherals();
    init_stepper_board();
    init_motion_control();
    init_lua_executor();

    /*pose_t current_pose = {
            .x = 470.0,
            .y = -1000.0,
            .theta = 0.0
        };

    vTaskDelay(200);*/
    while(1) {
        //pose_t lidar_pose;
        //refine_pose(&current_pose, &lidar_pose);
        vTaskDelay(100);
    }
}
