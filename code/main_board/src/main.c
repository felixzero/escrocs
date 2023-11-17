#include "system/i2c_master.h"
#include "system/modbus_rtu_master.h"
#include "system/spiffs.h"
#include "wireless/wifi.h"
#include "wireless/httpd.h"
#include "wireless/udp_logger.h"
#include "peripherals/stepper_board.h"
#include "peripherals/peripherals.h"
#include "peripherals/ultrasonic_board.h"
#include "peripherals/motor_board_v3.h"
#include "peripherals/user_interface.h"
#include "motion/motion_control.h"
#include "actions/strategy.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

void app_main() {
    ESP_LOGI("main", "Starting ESCRObot application...\n");
    init_user_interface();
    init_modbus_rtu_master();
    init_spiffs();
    init_wifi_system();
    init_http_server();
    vTaskDelay(200);

    init_ultrasonic_board();
    init_motor_board_v3();
    //init_motion_control(read_switch(GPIO_CHANNEL_SIDE));
    init_motion_control(false);
    init_lua_executor();
    init_udp_logger();

    vTaskDelay(portMAX_DELAY);
}
