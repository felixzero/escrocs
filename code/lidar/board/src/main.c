#include <esp_err.h>
#include <esp_log.h>

#include "parser.h"
#include "collision_handler.h"
#include "lidar.h"
#include "esp_i2c.h"

#include "../loca_lidar/amalgame.h"
#include "../loca_lidar/loca_lidar.h"
#include "../loca_lidar/pose_refinement.h"

#include "esp_heap_caps.h"
#include <sys/time.h>


#include "driver/gpio.h"
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    int level = gpio_get_level(gpio_num);
    ESP_LOGI("GPIO", "GPIO %i changed state to %i", (uint16_t) gpio_num, level);
}

void setup_gpio_interrupt(gpio_num_t gpio_num1, gpio_num_t gpio_num2) {
    // Configure the GPIO pin as input
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_ANYEDGE; // Interrupt on any edge
    io_conf.pin_bit_mask = (1ULL << gpio_num1) || (1ULL << gpio_num2);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    // Install the GPIO ISR service
    gpio_install_isr_service(0);

    // Hook the ISR handler
    gpio_isr_handler_add(gpio_num1, gpio_isr_handler, (void*) gpio_num1);
    gpio_isr_handler_add(gpio_num2, gpio_isr_handler, (void*) gpio_num2);
}

#define TAG "MAIN"
void app_main() {

    init_uart();
    xTaskCreate(update_amalgames_task, "update_amalgames_task", 2048, NULL, 8, NULL);
    xTaskCreate(i2c_slave_task, "i2c_slave_task", 2048, NULL, 9, NULL);
    //setup_gpio_interrupt(GPIO_NUM_3, GPIO_NUM_4);
    update_cone(0.0f, 0.7f);
    update_dist(500);
    for(;;) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        //ESP_LOGI(TAG, "closest_dist %i", has_obstacle());
    }



}