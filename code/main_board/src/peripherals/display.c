#include "display.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <stdint.h>
#include <stdarg.h>
#include <esp_log.h>
#include <esp_err.h>
#include <driver/gpio.h>
#include <rom/ets_sys.h>
#include <driver/gpio.h>
#include <driver/pcnt.h>
#include <limits.h>

#define NUMBER_OF_COLUMNS           16
#define LINE_OFFSET                 40
#define GLITCH_FILTER_NS            10000

#define GPIO_CHANNEL_LCD_BIT_4      26
#define GPIO_CHANNEL_LCD_BIT_5      27
#define GPIO_CHANNEL_LCD_BIT_6      32
#define GPIO_CHANNEL_LCD_BIT_7      33

#define GPIO_CHANNEL_LCD_RS         14
#define GPIO_CHANNEL_LCD_E          23

#define GPIO_CHANNEL_ENCODER_SW     5
#define GPIO_CHANNEL_ENCODER_DT     34
#define GPIO_CHANNEL_ENCODER_CLK    35

#define GPIO_CHANNEL_TRIGGER        13
#define GPIO_CHANNEL_SIDE           25
#define GPIO_CHANNEL_STATUS_LED     2

static void init_lcd_screen(void);
static void lcd_write_8bits(uint8_t value, bool rs_enable);
static void lcd_write_4bits(uint8_t value);
static void init_rotary_encoder(void);
static void init_gpio(void);

static pcnt_unit_t encoder_pcnt_unit = 0;
static SemaphoreHandle_t lcd_mutex;

void init_display(void)
{
    lcd_mutex = xSemaphoreCreateMutex();

    init_lcd_screen();
    init_rotary_encoder();
    init_gpio();
}

void lcd_printf(int row, const char *format_str, ...)
{
    va_list args;
    va_start(args, format_str);

    char buffer[NUMBER_OF_COLUMNS + 1];

    int message_length = vsnprintf(buffer, NUMBER_OF_COLUMNS + 1, format_str, args);
    va_end(args);

    xSemaphoreTake(lcd_mutex, portMAX_DELAY);

    // Move cursor
    lcd_write_8bits(row ? 0x80 + LINE_OFFSET : 0x80, 0);

    // Send string
    int i;
    for (i = 0; (i < message_length) && (i < NUMBER_OF_COLUMNS); i++) {
        lcd_write_8bits(buffer[i], 1);
    }
    for (; i < NUMBER_OF_COLUMNS; i++) {
        lcd_write_8bits(' ', 1);
    }

    xSemaphoreGive(lcd_mutex);
}


rotary_encoder_event_t poll_rotary_encoder_event(void)
{
    static int last_encoder_count = 0, last_switch_level = 1;
    int current_switch_level;
    int16_t current_count;

    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_get_counter_value(encoder_pcnt_unit, &current_count));
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

bool read_trigger_key_status(void)
{
    return gpio_get_level(GPIO_CHANNEL_TRIGGER);
}

void switch_status_led(bool state)
{
    gpio_set_level(GPIO_CHANNEL_STATUS_LED, state);
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
            | (1ULL << GPIO_CHANNEL_LCD_E)
        ),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&config);

    // Send initialization magic flags
    lcd_write_8bits(0x33, 0);
    vTaskDelay(1);
    lcd_write_8bits(0x32, 0);
    vTaskDelay(1);
    // Configure as 2-lines display, using 4-bits interface with 5x8 characters
    lcd_write_8bits(0x28, 0);
    vTaskDelay(1);
    // Activate display, remove cursor
    lcd_write_8bits(0x0C, 0);
    vTaskDelay(1);
    // Clear display
    lcd_write_8bits(0x01, 0);
    vTaskDelay(1);
    // Set left-to-right writing
    lcd_write_8bits(0x06, 0);
    vTaskDelay(1);
}

static void lcd_write_8bits(uint8_t value, bool rs_enable)
{
    gpio_set_level(GPIO_CHANNEL_LCD_RS, rs_enable);

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

    pcnt_config_t dev_config = {
        .pulse_gpio_num = GPIO_CHANNEL_ENCODER_CLK,
        .ctrl_gpio_num = GPIO_CHANNEL_ENCODER_DT,
        .channel = PCNT_CHANNEL_0,
        .unit = encoder_pcnt_unit,
        .pos_mode = PCNT_COUNT_DEC,
        .neg_mode = PCNT_COUNT_INC,
        .lctrl_mode = PCNT_MODE_REVERSE,
        .hctrl_mode = PCNT_MODE_KEEP,
        .counter_h_lim = INT16_MAX,
        .counter_l_lim = INT16_MIN
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_config(&dev_config));
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_set_filter_value(encoder_pcnt_unit, GLITCH_FILTER_NS * 80 / 1000));
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_filter_enable(encoder_pcnt_unit));
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_counter_pause(encoder_pcnt_unit));
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_counter_clear(encoder_pcnt_unit));
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_counter_resume(encoder_pcnt_unit));
}

static void init_gpio(void)
{
    gpio_config_t input_config = {
        .pin_bit_mask = (1ULL << GPIO_CHANNEL_TRIGGER) | (1ULL << GPIO_CHANNEL_SIDE),
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
