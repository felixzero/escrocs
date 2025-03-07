#include <esp_err.h>
#include <esp_log.h>
#include "driver/ledc.h"

#include "parser.h"
#include "collision_handler.h"
#include "lidar.h"
#include "esp_i2c.h"

#include "../loca_lidar/amalgame.h"
#include "../loca_lidar/loca_lidar.h"
#include "../loca_lidar/pose_refinement.h"

#include "esp_heap_caps.h"
#include <sys/time.h>


#define TAG "MAIN"
#define SPEED_LIDAR_PRCNT 40

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          21
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_8_BIT
#define LEDC_DUTY               256
#define LEDC_FREQUENCY          30000

void app_main() {
    // Configuration du timer LEDC
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Configuration du canal LEDC
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Valeur initiale du duty cycle
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    uint32_t duty = (uint32_t)((float)LEDC_DUTY * SPEED_LIDAR_PRCNT * 0.01);
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

    xTaskCreate(i2c_slave_task, "i2c_slave_task", 4096, NULL, 9, NULL);
    init_uart();
    xTaskCreate(update_amalgames_task, "update_amalgames_task", 2048, NULL, 8, NULL);
    xTaskCreate(printer_log_task, "printer_log_task", 2048, NULL, 12, NULL);
    update_cone(0.0f, 0.7f);
    update_dist(500);
    for(;;) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "closest_dist %i", has_obstacle());
    }



}