#include <driver/uart.h>
#include <esp_log.h>

#include "lidar/motor_control.h"
#include "lidar/uart_receiver.h"
#include "wireless/wifi.h"
#include "wireless/httpd.h"
#include "system/i2c_slave.h"
#include "localization/pose_refinement.h"

void app_main()
{
    vTaskDelay(200);
    init_wifi_system();
    init_http_server();
    init_motor_control();
    init_uart_receiver();
    init_pose_refinement();
    init_i2c_slave();

    set_beacon_position(0, 0.0, 0.0);
    set_beacon_position(1, 1200.0, 0.0);
    set_beacon_position(2, 0.0, -1000.0);

    pose_refinement_tuning_t tuning = {
        .reflectivity_threshold = 3500000,
        .max_acceptable_beacon_error = 200.0,
        .angle_offset = -0.917,
    };
    set_pose_refinement_tuning(&tuning);

    while (1) {
        vTaskDelay(100);
    }
}
