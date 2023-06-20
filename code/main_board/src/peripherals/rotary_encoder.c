#include "peripherals/rotary_encoder.h"

#include <driver/gpio.h>
#include <driver/pulse_cnt.h>
#include <limits.h>

#define GPIO_CHANNEL_SW 27
#define GPIO_CHANNEL_DT 26
#define GPIO_CHANNEL_CLK 32

#define GLITCH_FILTER_NS 10000

static pcnt_unit_handle_t pcnt_unit = NULL;

void init_rotary_encoder(void)
{
    gpio_config_t config = {
        .pin_bit_mask = (1ULL << GPIO_CHANNEL_SW),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&config);

    pcnt_unit_config_t unit_config = {
        .low_limit = INT16_MIN,
        .high_limit = INT16_MAX,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_new_unit(&unit_config, &pcnt_unit));

    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = GPIO_CHANNEL_CLK,
        .level_gpio_num = GPIO_CHANNEL_DT, 
    };
    pcnt_channel_handle_t pcnt_channel = NULL;
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_channel));

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        pcnt_channel_set_edge_action(pcnt_channel, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE)
    );
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        pcnt_channel_set_level_action(pcnt_channel, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE)
    );

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = GLITCH_FILTER_NS,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_enable(pcnt_unit));
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_start(pcnt_unit));
}

rotary_encoder_event_t poll_rotary_encoder_event(void)
{
    static int last_encoder_count = 0, last_switch_level = 1;
    int current_count, current_switch_level;

    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_get_count(pcnt_unit, &current_count));
    current_switch_level = gpio_get_level(GPIO_CHANNEL_SW);

    if ((current_switch_level == 0) && (last_switch_level == 1)) {
        last_switch_level = current_switch_level;
        return ROTARY_ENCODER_PRESSED;
    }
    last_switch_level = current_switch_level;

    if (current_count > last_encoder_count + 1) {
        last_encoder_count = current_count;
        return ROTARY_ENCODER_TURN_CCW;
    } else if (current_count < last_encoder_count - 1) {
        last_encoder_count = current_count;
        return ROTARY_ENCODER_TURN_CW;
    }

    return ROTARY_ENCODER_NO_EVENT;
}
