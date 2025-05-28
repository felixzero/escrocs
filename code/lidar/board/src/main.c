#include <esp_err.h>
#include <esp_log.h>
#include "driver/ledc.h"
#include "driver/gpio.h"

#include "parser.h"
#include "collision_handler.h"
#include "lidar.h"
#include "esp_i2c.h"
#include "task_priority.h"
#include "wifi.h"

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

    //Read GPIO 5 and 7
    //gpio_config_t io_conf = {
    //    .pin_bit_mask = (1ULL << GPIO_NUM_7),//(1ULL << GPIO_NUM_5) | (1ULL << GPIO_NUM_7),
    //    .mode = GPIO_MODE_INPUT,
    //    .pull_up_en = GPIO_PULLUP_ENABLE,
    //    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    //    .intr_type = GPIO_INTR_DISABLE,
    //};
    //gpio_config(&io_conf);

    gpio_config_t io_out = {
        .pin_bit_mask = (1ULL << GPIO_NUM_7),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_out);

    gpio_set_level(GPIO_NUM_5, 1);
 
    xTaskCreate(i2c_slave_task, "i2c_slave_task", 4096, NULL, I2C_TASK_PRIORITY, NULL);
    init_uart();
    xTaskCreate(update_amalgames_task, "update_amalgames_task", 2048, NULL, AMALGAME_TASK_PRIORITY, NULL);
    //xTaskCreate(printer_log_task, "printer_log_task", 2048, NULL, PRINTER_TASK_PRIORITY, NULL); //I2C Doesn't work without this task !
    update_cone(0.0f, 0.7f);
    update_dist(500);

    wifi_init_sta();

    for(;;) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        ESP_LOGI("", "obstacle main %i", has_obstacle());
        if(has_obstacle()) {
            gpio_set_level(GPIO_NUM_7, 0);
        }else {
            gpio_set_level(GPIO_NUM_7, 1);
        }
        //ESP_LOGI(TAG, "closest_dist %i", closest_obstacle_dist());
    }



}