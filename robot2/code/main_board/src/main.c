#include "i2c_master.h"
#include "motion_control.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
void prout();

void app_main() {
    printf("Starting application...\n");
    init_i2c_master();

    feedback_params_t pid = { 0.002, 0.0, -0.0 };
    wheel_geometry_t geometry = { .wheel_radius = 25.0, .robot_radius = 100.0 };
    init_motion_control(pid, geometry);

    int i = 0;
    while (1) {
        if (is_motion_done()) {
            if (i % 2 == 0) {
                set_motion_target(900.0, 0.0, 0.0);
            } else {
                set_motion_target(0.0, 0.0, 0.0);
            }
            i++;
        }
        motion_control_loop();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
