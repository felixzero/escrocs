#include "stepper.h"

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "precalculation.h"

struct acceleration_step stopped_step = {
    .period_prefactor = FASTEST_PERIOD_POSSIBLE - 1,
    .final_indice = 0,
    .current_indice = SQRT_DIFF_MAX_PRECALCULATED_VALUE,
    .indice_increment = 0,
    .next_step = &stopped_step
};

struct acceleration_step *stepper0_step = &stopped_step;
struct acceleration_step *stepper1_step = &stopped_step;
struct acceleration_step *stepper2_step = &stopped_step;

#define MAX_NUMBER_OF_STEPS 128
#define min(a, b) (((a) > (b)) ? (b) : (a))
#define abs_diff(a, b) (((a) > (b)) ? ((a) - (b)) : ((b) - (a)))

static void configure_timer(
    volatile uint8_t *TCCRA, volatile uint8_t *TCCRB,
    volatile uint16_t *ICR, volatile uint16_t *OCRA,
    volatile uint16_t *TCNT, volatile uint8_t *TIMSK
);

void init_stepper(void)
{
    // Configure control GPIO
    DDRB |= _BV(0) | _BV(1);
    DDRC |= _BV(0) | _BV(1) | _BV(2) | _BV(3);
    DDRD |= _BV(0) | _BV(1) | _BV(2);
    DDRE |= _BV(0) | _BV(1) | _BV(2);

    // 1/2 stepping mode
    PORTC |= _BV(0);

    configure_timer(&TCCR1A, &TCCR1B, &ICR1, &OCR1A, &TCNT1, &TIMSK1);
    configure_timer(&TCCR3A, &TCCR3B, &ICR3, &OCR3A, &TCNT3, &TIMSK3);
    configure_timer(&TCCR4A, &TCCR4B, &ICR4, &OCR4A, &TCNT4, &TIMSK4);
}

struct acceleration_step* build_acceleration_step(uint16_t initial_period, uint16_t final_period, uint16_t distance, uint8_t polarity)
{
    static uint8_t index_in_pool = 0;
    static struct acceleration_step step_pool[MAX_NUMBER_OF_STEPS];
    struct acceleration_step *result = &step_pool[index_in_pool];

    if (initial_period == final_period) {
        result->current_indice = SQRT_DIFF_MAX_PRECALCULATED_VALUE;
        result->indice_increment = 1;
        result->final_indice = SQRT_DIFF_MAX_PRECALCULATED_VALUE + distance;
        result->period_prefactor = initial_period;
    } else {
        uint16_t fastest_period = min(initial_period, final_period);
        uint16_t initial_indice = ((uint32_t)fastest_period * distance) / abs_diff(initial_period, final_period);
        uint16_t final_indice = initial_indice + distance;
        result->current_indice = (final_period < initial_period) ? initial_indice : final_indice;
        result->indice_increment = (final_period < initial_period) ? 1 : -1;
        result->final_indice = (final_period < initial_period) ? final_indice : initial_indice;
        result->period_prefactor = (((uint32_t)fastest_period) * 256) / pgm_read_word_near(sqrt_diff_precalc + final_indice - 1);
    }
    index_in_pool++;
    index_in_pool %= MAX_NUMBER_OF_STEPS;
    result->polarity = polarity;
    return result;
}

void start_motion_immediately(struct acceleration_step* channel0, struct acceleration_step* channel1, struct acceleration_step* channel2)
{
    stepper0_step = channel0;
    stepper1_step = channel1;
    stepper2_step = channel2;
}

void stop_motion_immediately(void)
{
    stepper0_step = &stopped_step;
    stepper1_step = &stopped_step;
    stepper2_step = &stopped_step;
}

static void configure_timer(
    volatile uint8_t *TCCRA, volatile uint8_t *TCCRB,
    volatile uint16_t *ICR, volatile uint16_t *OCRA,
    volatile uint16_t *TCNT, volatile uint8_t *TIMSK
)
{
    // Activate output compare A, clear on compare, fast PWM to ICR1
    // Prescaler to 16MHz / 256 = 62.5kHz
    *TCCRA = _BV(COM1A1) | _BV(WGM11);
    *TCCRB = _BV(WGM13) | _BV(WGM12) | _BV(CS12);
    *ICR = FASTEST_PERIOD_POSSIBLE - 1;
    *OCRA = FASTEST_PERIOD_POSSIBLE;
    *TCNT = 0;

    // Enable overflow interrupts
    cli();
    *TIMSK |= _BV(TOIE1);
    sei();
}
