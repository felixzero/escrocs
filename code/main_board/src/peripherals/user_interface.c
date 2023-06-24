#include "user_interface.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <stdarg.h>
#include <rom/ets_sys.h>
#include <driver/gpio.h>
#include <driver/pulse_cnt.h>
#include <limits.h>

#define GPIO_CHANNEL_LCD_BIT_4 14
#define GPIO_CHANNEL_LCD_BIT_5 13
#define GPIO_CHANNEL_LCD_BIT_6 15
#define GPIO_CHANNEL_LCD_BIT_7 12

#define GPIO_CHANNEL_LCD_RS 32 
#define GPIO_CHANNEL_LCD_RW 26
#define GPIO_CHANNEL_LCD_E 27

#define GPIO_CHANNEL_ENCODER_SW 27
#define GPIO_CHANNEL_ENCODER_DT 26
#define GPIO_CHANNEL_ENCODER_CLK 32

#define NUMBER_OF_COLUMNS 16
#define LINE_OFFSET 40

#define GLITCH_FILTER_NS 10000

static pcnt_unit_handle_t encoder_pcnt_unit = NULL;

static void init_gpio(void);
static void init_lcd_screen(void);
static void lcd_write_8bits(uint8_t value, bool rs_enable, bool rw_enable);
static void lcd_write_4bits(uint8_t value);
static void init_rotary_encoder(void);

void init_user_interface(void)
{
    init_gpio();
    init_lcd_screen();
    init_rotary_encoder();
}

void lcd_printf(int row, const char *format_str, ...)
{
    va_list args;
    va_start(args, format_str);

    char buffer[NUMBER_OF_COLUMNS + 1];

    int message_length = vsnprintf(buffer, NUMBER_OF_COLUMNS + 1, format_str, args);
    va_end(args);

    // Move cursor
    lcd_write_8bits(row ? 0x80 + LINE_OFFSET : 0x80, 0, 0);

    // Send string
    int i;
    for (i = 0; (i < message_length) && (i < NUMBER_OF_COLUMNS); i++) {
        lcd_write_8bits(buffer[i], 1, 0);
    }
    for (; i < NUMBER_OF_COLUMNS; i++) {
        lcd_write_8bits(' ', 1, 0);
    }
}

rotary_encoder_event_t poll_rotary_encoder_event(void)
{
    static int last_encoder_count = 0, last_switch_level = 1;
    int current_count, current_switch_level;

    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_get_count(encoder_pcnt_unit, &current_count));
    current_switch_level = gpio_get_level(GPIO_CHANNEL_ENCODER_SW);

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

bool read_switch(int channel)
{
    return gpio_get_level(channel);
}

void switch_on_led(void)
{
    gpio_set_level(GPIO_CHANNEL_STATUS_LED, 1);
}

static void init_gpio(void)
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
        .pin_bit_mask = (1ULL << GPIO_CHANNEL_STATUS_LED),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&output_config);
}

static void init_lcd_screen(void)
{
    gpio_config_t config = {
        .pin_bit_mask = (
            (1ULL << GPIO_CHANNEL_LCD_BIT_4)
            | (1ULL << GPIO_CHANNEL_LCD_BIT_5)
            | (1ULL << GPIO_CHANNEL_LCD_BIT_6)
            | (1ULL << GPIO_CHANNEL_LCD_BIT_7)
            | (1ULL << GPIO_CHANNEL_LCD_RS)
            | (1ULL << GPIO_CHANNEL_LCD_RW)
            | (1ULL << GPIO_CHANNEL_LCD_E)
        ),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&config);

    // Send initialization magic flags
    lcd_write_8bits(0x33, 0, 0);
    vTaskDelay(1);
    lcd_write_8bits(0x32, 0, 0);
    vTaskDelay(1);
    // Configure as 2-lines display, using 4-bits interface with 5x8 characters
    lcd_write_8bits(0x28, 0, 0);
    vTaskDelay(1);
    // Activate display, remove cursor
    lcd_write_8bits(0x0C, 0, 0);
    vTaskDelay(1);
    // Clear display
    lcd_write_8bits(0x01, 0, 0);
    vTaskDelay(1);
    // Set left-to-right writing
    lcd_write_8bits(0x06, 0, 0);
    vTaskDelay(1);
}

static void lcd_write_8bits(uint8_t value, bool rs_enable, bool rw_enable)
{
    gpio_set_level(GPIO_CHANNEL_LCD_RS, rs_enable);
    gpio_set_level(GPIO_CHANNEL_LCD_RW, rw_enable);

    lcd_write_4bits(value >> 4);
    lcd_write_4bits(value & 0x0F);
}

static void lcd_write_4bits(uint8_t value)
{
    gpio_set_level(GPIO_CHANNEL_LCD_BIT_4, (value & 0x01) != 0);
    gpio_set_level(GPIO_CHANNEL_LCD_BIT_5, (value & 0x02) != 0);
    gpio_set_level(GPIO_CHANNEL_LCD_BIT_6, (value & 0x04) != 0);
    gpio_set_level(GPIO_CHANNEL_LCD_BIT_7, (value & 0x08) != 0);

    gpio_set_level(GPIO_CHANNEL_LCD_E, 1);
    ets_delay_us(10);
    gpio_set_level(GPIO_CHANNEL_LCD_E, 0);
    ets_delay_us(10);
}

static void init_rotary_encoder(void)
{
    gpio_config_t config = {
        .pin_bit_mask = (1ULL << GPIO_CHANNEL_ENCODER_SW),
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
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_new_unit(&unit_config, &encoder_pcnt_unit));

    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = GPIO_CHANNEL_ENCODER_CLK,
        .level_gpio_num = GPIO_CHANNEL_ENCODER_DT, 
    };
    pcnt_channel_handle_t pcnt_channel = NULL;
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_new_channel(encoder_pcnt_unit, &chan_config, &pcnt_channel));

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        pcnt_channel_set_edge_action(pcnt_channel, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE)
    );
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        pcnt_channel_set_level_action(pcnt_channel, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE)
    );

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = GLITCH_FILTER_NS,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_set_glitch_filter(encoder_pcnt_unit, &filter_config));

    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_enable(encoder_pcnt_unit));
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_start(encoder_pcnt_unit));
}
