#include "lcd_screen.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include <stdarg.h>
#include <rom/ets_sys.h>

#define GPIO_CHANNEL_BIT_4 14
#define GPIO_CHANNEL_BIT_5 13
#define GPIO_CHANNEL_BIT_6 15
#define GPIO_CHANNEL_BIT_7 12

#define GPIO_CHANNEL_RS 32 
#define GPIO_CHANNEL_RW 26
#define GPIO_CHANNEL_E 27

#define NUMBER_OF_COLUMNS 16
#define LINE_OFFSET 40

static void write_8bits(uint8_t value, bool rs_enable, bool rw_enable);
static void write_4bits(uint8_t value);

void init_lcd_screen(void)
{
    gpio_config_t config = {
        .pin_bit_mask = (
            (1ULL << GPIO_CHANNEL_BIT_4)
            | (1ULL << GPIO_CHANNEL_BIT_5)
            | (1ULL << GPIO_CHANNEL_BIT_6)
            | (1ULL << GPIO_CHANNEL_BIT_7)
            | (1ULL << GPIO_CHANNEL_RS)
            | (1ULL << GPIO_CHANNEL_RW)
            | (1ULL << GPIO_CHANNEL_E)
        ),
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&config);

    // Send initialization magic flags
    write_8bits(0x33, 0, 0);
    vTaskDelay(1);
    write_8bits(0x32, 0, 0);
    vTaskDelay(1);
    // Configure as 2-lines display, using 4-bits interface with 5x8 characters
    write_8bits(0x28, 0, 0);
    vTaskDelay(1);
    // Activate display, remove cursor
    write_8bits(0x0C, 0, 0);
    vTaskDelay(1);
    // Clear display
    write_8bits(0x01, 0, 0);
    vTaskDelay(1);
    // Set left-to-right writing
    write_8bits(0x06, 0, 0);
    vTaskDelay(1);
}

void lcd_printf(int row, const char *format_str, ...)
{
    va_list args;
    va_start(args, format_str);

    char buffer[NUMBER_OF_COLUMNS + 1];

    int message_length = vsnprintf(buffer, NUMBER_OF_COLUMNS + 1, format_str, args);
    va_end(args);

    // Move cursor
    write_8bits(row ? 0x80 + LINE_OFFSET : 0x80, 0, 0);

    // Send string
    int i;
    for (i = 0; (i < message_length) && (i < NUMBER_OF_COLUMNS); i++) {
        write_8bits(buffer[i], 1, 0);
    }
    for (; i < NUMBER_OF_COLUMNS; i++) {
        write_8bits(' ', 1, 0);
    }
}

static void write_8bits(uint8_t value, bool rs_enable, bool rw_enable)
{
    gpio_set_level(GPIO_CHANNEL_RS, rs_enable);
    gpio_set_level(GPIO_CHANNEL_RW, rw_enable);

    write_4bits(value >> 4);
    write_4bits(value & 0x0F);
}

static void write_4bits(uint8_t value)
{
    gpio_set_level(GPIO_CHANNEL_BIT_4, (value & 0x01) != 0);
    gpio_set_level(GPIO_CHANNEL_BIT_5, (value & 0x02) != 0);
    gpio_set_level(GPIO_CHANNEL_BIT_6, (value & 0x04) != 0);
    gpio_set_level(GPIO_CHANNEL_BIT_7, (value & 0x08) != 0);

    gpio_set_level(GPIO_CHANNEL_E, 1);
    ets_delay_us(10);
    gpio_set_level(GPIO_CHANNEL_E, 0);
    ets_delay_us(10);
}
