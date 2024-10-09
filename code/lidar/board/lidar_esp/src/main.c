#include <esp_log.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ld06_driver.h"

#define RX_LIDAR_PIN 20

void app_main() {
    vTaskDelay(1000/portTICK_PERIOD_MS);
    ESP_LOGI("lidar", "hello world");
    init_lidar(RX_LIDAR_PIN);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    ESP_LOGI("lidar", "init done");

    int length = 0;
    while(1) {
        }
        else {
            vTaskDelay(10/portTICK_PERIOD_MS);
        }

    }

    
}