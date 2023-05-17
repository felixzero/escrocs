#include "global_definitions.h"
#include "led_strip.h"
#include "ultrasound.h"
#include "controller.h"

#include <stdbool.h>
#include <avr/io.h>

#define DEFAULT_CRITICAL_DISTANCE 100
#define REPETITION_RATE 156 // 156 ticks = 20 ms

bool enabled_channels[NUMBER_OF_US];
uint8_t ultrasound_distances[NUMBER_OF_US];
uint8_t critical_threshold_distance;

void init_controller(void)
{
    for (uint8_t i = 0; i < NUMBER_OF_US; ++i) {
        enabled_channels[i] = true;
        ultrasound_distances[i] = 0;
        critical_threshold_distance = DEFAULT_CRITICAL_DISTANCE;
    }

    // Enable repetition timer with /1024 prescaler (7.81 kHz)
    TCCR0 = _BV(CS02) | _BV(CS00);
}

bool is_path_obstructed(void)
{
    for (uint8_t i = 0; i < NUMBER_OF_US; ++i) {
        if ((ultrasound_distances[i] < critical_threshold_distance) && enabled_channels[i]) {
            return true;
        }
    }
    return false;
}

void perform_ultrasound_scan(yield_fn yield)
{
    for (uint8_t i = 0; i < NUMBER_OF_LED; ++i) {
        if (!enabled_channels[i]) {
            yield();
            continue;
        }
        TCNT0 = 0;
        ultrasound_distances[i] = pulse_ultrasound(i);
        yield();

        while (TCNT0 < REPETITION_RATE);
        yield();
    }
    // FIXME: Causes race conditions with the I2C bus; to be put back when fixed
    //write_led_strip_values(ultrasound_distances, enabled_channels, critical_threshold_distance);
    //led_strip_bit_banging();
}
