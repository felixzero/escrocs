#include "system/i2c_master.h"
#include "system/spiffs.h"
#include "wireless/wifi.h"
#include "wireless/httpd.h"
#include "peripherals/peripherals.h"
#include "peripherals/stepper_board.h"
#include "peripherals/lidar_board.h"
#include "motion/motion_control.h"
#include "actions/strategy.h"
#include "motion/motor_board.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

void app_main() {
    ESP_LOGI("main", "Starting ESCRObot application...\n");
    init_i2c_master();
    init_spiffs();
    init_wifi_system();
    init_http_server();
    //init_peripherals();
    //init_stepper_board();
    //init_motion_control();
    //init_lua_executor();

    //set_stepper_board_pump(0, false);

    //set_peripherals_pump(1, true);
    //set_peripherals_servo_channel(3, 1500);

    //set_speed_targets(-400.0, 400.0, 0.0);

    //move_peripherals_motor(1, -1000, 0.7);

    vTaskDelay(4000 / portTICK_PERIOD_MS);
    while(1) {
        //ESP_LOGI("main", "%d", read_peripherals_motor_input(0, 1));
        /*float yaw, pitch, roll;
        read_gyroscope(&yaw, &pitch, &roll);
        ESP_LOGI("DEBUG", "Yaw: %f", yaw);*/
        //pose_t pose = get_current_pose();
        //ESP_LOGI("Main", "Pose: %f %f %f", pose.x, pose.y, pose.theta);

        /*vTaskDelay(1);
        uint8_t reg = 0x07, output;
        send_to_i2c(I2C_PORT_MOTOR, 0x0D, &reg, 1);
        request_from_i2c(I2C_PORT_MOTOR, 0x0D, &output, 1);
        ESP_LOGI("main", "%x", output);*/
        pose_t guess_pose = {
            .x = 470.0,
            .y = -1000.0,
            .theta = 0.0
        };
        pose_t refined_pose;
        refine_pose(&guess_pose, &refined_pose);
        vTaskDelay(1);
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
