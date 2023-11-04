#include "global_definitions.h"
#include "led_strip.h"
#include "ultrasound.h"
#include "controller.h"

#include <stdbool.h>
#include <avr/io.h>

#define DEFAULT_CRITICAL_DISTANCE_MM        400
#define DEFAULT_SAFE_DISTANCE_MM            1000

#define DEFAULT_REPETITION_PERIOD_MS        20
#define REPETION_TIMER_FREQUENCY            (F_CPU / 1024)
#define REPETITION_PERIOD_MS_TO_TICKS(t)    (uint16_t)((uint32_t)(t) * REPETION_TIMER_FREQUENCY / 1000)

#define DEFAULT_TIMEOUT_DISTANCE_MM         2000
#define SPEED_OF_SOUND_DM_PER_S             3432

#define US_FROM_DISTANCE(d_mm)              (uint16_t)((uint32_t)(d_mm) * 20000 / SPEED_OF_SOUND_DM_PER_S)
#define DISTANCE_FROM_US(us_us)             (uint16_t)((uint32_t)(us_us) * SPEED_OF_SOUND_DM_PER_S / 20000)

bool enabled_channels[NUMBER_OF_US];
uint16_t ultrasound_distances[NUMBER_OF_US];

uint16_t critical_threshold_distance = DEFAULT_CRITICAL_DISTANCE_MM;
uint16_t safe_distance = DEFAULT_SAFE_DISTANCE_MM;
uint8_t repetition_period = DEFAULT_REPETITION_PERIOD_MS;
uint16_t timeout_distance = DEFAULT_TIMEOUT_DISTANCE_MM;
enum controller_state_t controller_state = ULTRASOUND_BOARD_IDLE;

static void perform_ultrasound_scan(void);
static void update_led_ribbon(void);

#define LED_STATUS_BUSY() (PORTB |= _BV(1))
#define LED_STATUS_IDLE() (PORTB &= ~_BV(1))

void init_controller(void)
{
    // Set status LED as output
    DDRB |= _BV(1);

    for (uint8_t i = 0; i < NUMBER_OF_US; ++i) {
        enabled_channels[i] = true;
        ultrasound_distances[i] = 0;
    }

    // Enable repetition timer with /1024 prescaler (7.81 kHz)
    TCCR0 = _BV(CS02) | _BV(CS00);
}

bool is_path_obstructed(void)
{
    for (uint8_t i = 0; i < NUMBER_OF_US; ++i) {
        if ((ultrasound_distances[i] < US_FROM_DISTANCE(critical_threshold_distance)) && enabled_channels[i]) {
            return true;
        }
    }
    return false;
}

void run_controller_state_machine(void)
{
    switch (controller_state) {
    case ULTRASOUND_BOARD_IDLE:
        LED_STATUS_IDLE();
        return;
    case ULTRASOUND_BOARD_SCANNING:
        LED_STATUS_BUSY();
        perform_ultrasound_scan();
        controller_state = ULTRASOUND_BOARD_IDLE;
        return;
    case ULTRASOUND_BOARD_UPDATING_LED:
        LED_STATUS_BUSY();
        update_led_ribbon();
        controller_state = ULTRASOUND_BOARD_IDLE;
        return;
    case ULTRASOUND_BOARD_FULL_UPDATE:
        LED_STATUS_BUSY();
        perform_ultrasound_scan();
        controller_state = ULTRASOUND_BOARD_UPDATING_LED;
    }
}

static void perform_ultrasound_scan(void)
{
    for (uint8_t i = 0; i < NUMBER_OF_LED; ++i) {
        if (!enabled_channels[i]) {
            continue;
        }
        TCNT0 = 0;

        uint16_t value = ULTRASOUND_TIMEOUT;
        if (pulse_ultrasound(i, &value, US_FROM_DISTANCE(timeout_distance))) {
            value = DISTANCE_FROM_US(value);
        }
        ultrasound_distances[i] = value;

        while (TCNT0 < REPETITION_PERIOD_MS_TO_TICKS(repetition_period));
    }
}

static void update_led_ribbon(void)
{
    write_led_strip_values(ultrasound_distances, enabled_channels, critical_threshold_distance, safe_distance);
    led_strip_bit_banging();
}
