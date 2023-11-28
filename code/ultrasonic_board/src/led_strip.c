#include "led_strip.h"
#include "global_definitions.h"

#include <avr/io.h>

const uint8_t low_color[] = { 255, 80, 0 };
const uint8_t high_color[] = { 0, 255, 0 };
const uint8_t critical_color[] = { 255, 0, 0 };

extern volatile uint8_t led_strip_payload[];

static uint8_t linear_interpolation(uint8_t a, uint8_t b, uint16_t t, uint16_t max_value)
{
    if (t >= max_value) {
        return b;
    }
    return ((uint32_t)a * (max_value - t) + b * t) / (max_value + 1);
}

void init_led_strip(void)
{
    DDRB |= LED_PIN;

    for (int i = 0; i < NUMBER_OF_LED; ++i) {
        led_strip_payload[3 * i + 0] = 0;
        led_strip_payload[3 * i + 1] = 0;
        led_strip_payload[3 * i + 2] = 0;
    }
}

void write_led_strip_values(uint16_t *values, bool *enabled, uint16_t critical_threshold, uint16_t graphical_max_value)
{
    for (int i = 0; i < NUMBER_OF_LED; ++i) {
        if (!enabled[i]) {
            led_strip_payload[3 * i + 0] = 0;
            led_strip_payload[3 * i + 1] = 0;
            led_strip_payload[3 * i + 2] = 255;
            continue;
        }

        if (values[i] < critical_threshold) {
            led_strip_payload[3 * i + 0] = critical_color[1];
            led_strip_payload[3 * i + 1] = critical_color[0];
            led_strip_payload[3 * i + 2] = critical_color[2];
        } else {
            led_strip_payload[3 * i + 0] = linear_interpolation(
                low_color[1], high_color[1], values[i] - critical_threshold, graphical_max_value
                );
            led_strip_payload[3 * i + 1] = linear_interpolation(
                low_color[0], high_color[0], values[i] - critical_threshold, graphical_max_value
                );
            led_strip_payload[3 * i + 2] = linear_interpolation(
                low_color[2], high_color[2], values[i] - critical_threshold, graphical_max_value
                );
        }
    }
}
