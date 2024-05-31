#include "system/i2c_master.h"
#include "system/modbus_rtu_master.h"
#include "system/spiffs.h"
#include "system/user_interface.h"
#include "wireless/wifi.h"
#include "wireless/httpd.h"
#include "wireless/udp_logger.h"
#include "peripherals/stepper_board.h"
#include "peripherals/peripherals.h"
#include "peripherals/ultrasonic_board.h"
#include "peripherals/motor_board_v3.h"
#include "peripherals/display.h"
#include "controllers/motion_control.h"
#include "controllers/us_control.h"
#include "actions/strategy.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#define MAX_STRATEGY_DISPLAY        8
#define STRATEGY_DISPLAYED_LENGTH   16

void app_main() {
    ESP_LOGI("main", "Starting ESCRObot application...\n");

    init_spiffs();
    init_wifi_system();
    init_http_server();
    vTaskDelay(pdMS_TO_TICKS(500));
    init_modbus_rtu_master();
    //init_udp_logger();

    int is_reversed = 0;
    int picked_strategy = 0;

    ESP_LOGI("main", "init_done ");
    init_lua_executor(is_reversed);
    pick_strategy_by_spiffs_index(picked_strategy);

    vTaskDelay(portMAX_DELAY);
}
