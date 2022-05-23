#include "system/i2c_master.h"
#include "system/spiffs.h"
#include "wireless/wifi.h"
#include "wireless/httpd.h"
#include "wireless/udp_logger.h"
#include "peripherals/stepper_board.h"
#include "peripherals/gpio.h"
#include "peripherals/peripherals.h"
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
    init_stepper_board();
    init_gpio();

#ifdef CONFIG_ESP_ROBOT_HOLONOMIC
    init_peripherals();
    set_peripherals_servo_channel(0, 6500);
    set_peripherals_servo_channel(1, 6500);
    set_peripherals_servo_channel(2, 6500);
#endif

    init_motion_control(read_switch(GPIO_CHANNEL_SIDE));
    init_lua_executor();
    init_udp_logger();

    while(1) {
        vTaskDelay(100);
    }
}
