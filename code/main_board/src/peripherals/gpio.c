#include "peripherals/gpio.h"

#include <driver/gpio.h>
#include <driver/ledc.h>

#define GPIO_CHANNEL_BUZZ 14

void init_gpio(void)
{
    gpio_config_t input_config = {
        .pin_bit_mask = (1ULL << GPIO_CHANNEL_TRIGGER) | (1ULL << GPIO_CHANNEL_SIDE) \
            | (1ULL << GPIO_CHANNEL_BA) | (1ULL << GPIO_CHANNEL_BB),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&input_config);

    gpio_config_t output_config = {
        .pin_bit_mask = ((1ULL << GPIO_CHANNEL_STATUS_LED) || (1ULL << GPIO_CHANNEL_BUZZ)),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&output_config);

    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 3000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_timer_config(&ledc_timer));
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = GPIO_CHANNEL_BUZZ,
        .duty = 0,
        .hpoint = 0
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_channel_config(&ledc_channel));
}

bool read_switch(int channel)
{
    return gpio_get_level(channel);
}

void switch_on_led(void)
{
    gpio_set_level(GPIO_CHANNEL_STATUS_LED, 1);
}

void switch_buzzer_on(void)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 4095));
    ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
}

void switch_buzzer_off(void)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0));
    ESP_ERROR_CHECK_WITHOUT_ABORT(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
}