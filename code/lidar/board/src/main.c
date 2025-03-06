#include <esp_err.h>
#include <esp_log.h>

#include "parser.h"
#include "collision_handler.h"
#include "lidar.h"
#include "i2c.h"

#include "../loca_lidar/amalgame.h"
#include "../loca_lidar/loca_lidar.h"
#include "../loca_lidar/pose_refinement.h"

#include "esp_heap_caps.h"
#include <sys/time.h>




#define TAG "MAIN"
void app_main() {

    init_uart();
    xTaskCreate(update_amalgames_task, "update_amalgames_task", 2048, NULL, 8, NULL);
    xTaskCreate(i2c_slave_task, "i2c_slave_task", 2048, NULL, 8, NULL);
    update_cone(0.0f, 0.7f);
    update_dist(500);
    for(;;) {
        vTaskDelay(200 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "closest_dist %i", has_obstacle());
    }



}