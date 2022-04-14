#include "lidar/motor_control.h"
#include "system/task_priority.h"

#include <esp_log.h>
#include <driver/ledc.h>
#include <esp_err.h>
#include <stdbool.h>

#define TAG "Motor"
#define MOTOR_GPIO_OUTPUT 4
#define TARGET_SPEED 300.0
#define INITIAL_DUTY 70
#define MAX_DUTY 127
#define ADJUSTMENT_PERIOD 2000

TaskHandle_t motor_control_task = NULL;

static void motor_control_task_handle(void *parameters);

void init_motor_control(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_8_BIT,
        .freq_hz          = 5000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = MOTOR_GPIO_OUTPUT,
        .duty           = INITIAL_DUTY,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    xTaskCreate(motor_control_task_handle, "motor_control", TASK_STACK_SIZE, NULL, MOTOR_CONTROL_PRIORITY, &motor_control_task);
}

static void motor_control_task_handle(void *parameters)
{
    while (true) {
        uint32_t raw_motor_speed = 0;
        xTaskNotifyWait(0, 0, &raw_motor_speed, pdMS_TO_TICKS(1000));
        float motor_speed = *(float*)&raw_motor_speed;
        if (raw_motor_speed == 0) {
            ESP_LOGW(TAG, "Could not retrieve motor speed");
            continue;
        }

        uint32_t duty = ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        duty += (motor_speed > TARGET_SPEED) ? -1 : 1;
        if (duty > MAX_DUTY) {
            duty = MAX_DUTY;
        }

        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
        ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
        vTaskDelay(pdMS_TO_TICKS(ADJUSTMENT_PERIOD));
    }
}
