#include "system/i2c_master.h"
#include "system/modbus_rtu_master.h"
#include "system/spiffs.h"
#include "system/user_interface.h"
#include "wireless/wifi.h"
#include "wireless/httpd.h"
#include "wireless/udp_logger.h"
#include "peripherals/stepper_board.h"
#include "peripherals/peripherals.h"
#include "peripherals/ld06_board.h"
#include "peripherals/motor_board.h"
#include "peripherals/display.h"
#include "controllers/motion_control.h"
#include "controllers/avoidance_control.h"
#include "actions/strategy.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#define MAX_STRATEGY_DISPLAY        8
#define STRATEGY_DISPLAYED_LENGTH   16

void app_main() {
    ESP_LOGI("main", "Starting ESCRObot application...\n");
    init_display();
    //display_initialization_status("Modbus", init_modbus_rtu_master());
    display_initialization_status("I2C", init_i2c_master());
    display_initialization_status("SPIFFS", init_spiffs());
    display_initialization_status("Wi-Fi", init_wifi_system());
    display_initialization_status("HTTP", init_http_server());
    vTaskDelay(pdMS_TO_TICKS(500));
    init_udp_logger();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_err_t ld06_board_ret = ESP_FAIL;
    //while (ld06_board_ret != ESP_OK) //Wait for lidar to boot up and make a full scan
    //{
    //    ld06_board_ret = init_ld06_board();
    //    ESP_LOGI("main", "ld06 state : %s", esp_err_to_name(ld06_board_ret));
    //    vTaskDelay(200/portTICK_PERIOD_MS);
    //}
    
    display_initialization_status("mockup avoidance", init_mockup());
    //display_initialization_status("Ultrasonic", init_ultrasonic_board());
    //display_initialization_status("US control", init_us_controller());
    display_initialization_status("Motor board", init_motor_board());
    display_initialization_status("Motion ctrl", init_motion_control(false));//is_reversed));
    display_initialization_status("Peripherals", init_peripherals());
    ESP_LOGI("main", "2nd Initialization done!\n");

    char *table_sides[] = { "Left", "Right" };
    int is_reversed = menu_pick_item("Table side", table_sides, 2);

    //Reset servo to their positions
    set_peripherals_servo_channel(0, 8500);

    char *strategies[MAX_STRATEGY_DISPLAY];
    int number_of_strategies = MAX_STRATEGY_DISPLAY;
    for (int index = 0; index < MAX_STRATEGY_DISPLAY; ++index) {
        char *strategy = malloc(STRATEGY_DISPLAYED_LENGTH);
        if (!list_spiffs_files(index, strategy, STRATEGY_DISPLAYED_LENGTH)) {
            free(strategy);
            number_of_strategies = index;
            break;
        }
        strategies[index] = strategy;
    }
    int picked_strategy = menu_pick_item("Strategy", strategies, number_of_strategies);
    for (int index = 0; index < number_of_strategies; ++index) {
        free(strategies[index]);
    }
    lcd_printf(0, "Side: %s", table_sides[is_reversed]);

    init_lua_executor(is_reversed);
    pick_strategy_by_spiffs_index(picked_strategy);

}
