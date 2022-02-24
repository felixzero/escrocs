#include "steppers.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define NUMBER_OF_CHANNELS 3
#define min(a, b) ((a) <= (b) ? (a) : (b))
#define max(a, b) ((a) >= (b) ? (a) : (b))

static volatile int16_t stepper_positions[NUMBER_OF_CHANNELS] = { 0 };
static int16_t stepper_targets[NUMBER_OF_CHANNELS] = { 0 };
static uint8_t stepper_target_pulse_periods[NUMBER_OF_CHANNELS] = { 0 };
static uint8_t stepper_target_acceleration_periods[NUMBER_OF_CHANNELS] = { 0 };
static uint8_t stepper_target_initial_positions[NUMBER_OF_CHANNELS] = { 0 };

static volatile uint8_t *ocrxa_registers[] = { &OCR0A, &OCR1AL, &OCR2A };
static volatile uint8_t *ocrxb_registers[] = { &OCR0B, &OCR1BL, &OCR2B };
static const uint8_t home_switch_channels[] = { _BV(4), _BV(6), _BV(7) };

static inline void timer_overflow_interrupt(uint8_t channel);

void init_steppers(void)
{
    // Configure PWM pins as output
    DDRB |= _BV(2);
    DDRD |= _BV(3) | _BV(5);

    // Configure data direction pins as output
    // And set CW by default
    DDRC |= _BV(0) | _BV(1) | _BV(2);
    PORTC |= _BV(0) | _BV(1) | _BV(2);

    // Configure all prescalers to /1024
    // Yielding 8 kHz PWM output (in phase correct mode)
    TCCR0B = _BV(CS02) | _BV(CS00) | _BV(WGM02);
    TCCR1B = _BV(CS12) | _BV(CS10) | _BV(WGM12);
    TCCR2B = _BV(CS22) | _BV(CS20) | _BV(WGM22); 

    // Set all channels in phase correct PWM to OCRxA and activate PWM on port B
    // This yields to half-frequency impulses at a frequency set by OCRxA
    TCCR0A = _BV(COM0B1) | _BV(WGM00);
    TCCR1A = _BV(COM1B1) | _BV(WGM10);
    TCCR2A = _BV(COM2B1) | _BV(WGM20);

    // Set pulse period to a slow value by default (OCRxA = 255)
    // And disable the pulse completly (OCRxB = 0)
    OCR0A = OCR1A = OCR2A = 255;
    OCR0B = OCR1BL = OCR2B = 0;

    // Enable overflow interrupts on all timers
    cli();
    TIMSK0 = _BV(TOIE0);
    TIMSK1 = _BV(TOIE1);
    TIMSK2 = _BV(TOIE2);
    sei();

    // Configure home switches as input pull-up
    for (uint8_t i = 0; i < NUMBER_OF_CHANNELS; ++i) {
        DDRD &= ~home_switch_channels[i];
        PORTD |= home_switch_channels[i];
        PCMSK0 |= home_switch_channels[i];
    }

    // Enable pin change interrupt on home switches
    cli();
    PCICR |= _BV(PCIE0);
    sei();
}

int16_t stepper_position(uint8_t channel)
{
    if (channel >= NUMBER_OF_CHANNELS) {
        return 0;
    }
    return stepper_positions[channel];
}

void overwrite_stepper_position(uint8_t channel, int16_t position)
{
    if (channel >= NUMBER_OF_CHANNELS) {
        return;
    }
    stepper_positions[channel] = position;
}

void move_stepper(uint8_t channel, int16_t position, uint8_t pulse_period, uint8_t acceleration_period)
{
    if ((channel > NUMBER_OF_CHANNELS) || position == stepper_positions[channel]) {
        return;
    }

    // Set target and direction bit
    stepper_target_initial_positions[channel] = stepper_positions[channel];
    stepper_targets[channel] = position;
    if (position > stepper_positions[channel]) {
        PORTC |= _BV(channel);
    } else {
        PORTC &= ~_BV(channel);
    }

    // Set speed
    stepper_target_pulse_periods[channel] = pulse_period;
    stepper_target_acceleration_periods[channel] = acceleration_period;
    *(ocrxa_registers[channel]) = 255;

    // Enable motion
    *(ocrxb_registers[channel]) = 1;
}

void stop_motion(uint8_t channel)
{
    *(ocrxb_registers[channel]) = 0;
}

bool is_stepper_in_motion(uint8_t channel)
{
    return *(ocrxb_registers[channel]);
}

ISR(TIMER0_OVF_vect) {
    timer_overflow_interrupt(0);
}

ISR(TIMER1_OVF_vect) {
    timer_overflow_interrupt(1);
}

ISR(TIMER2_OVF_vect) {
    timer_overflow_interrupt(2);
}

ISR(PCINT0_vect) {
    // Cut motor and set home value if home switch is closed
    for (uint8_t i = 0; i < NUMBER_OF_CHANNELS; ++i) {
        if (!(PIND & home_switch_channels[i])) {
            *(ocrxb_registers[i]) = 0;
            stepper_positions[i] = 0;
        }
    }
}

static inline void timer_overflow_interrupt(uint8_t channel) {
    // Increment position counter if in motion
    if (*(ocrxb_registers[channel])) {
        stepper_positions[channel] += (PORTC & _BV(channel)) ? 1 : -1;

        // Set speed for acceleration ramp
        if (stepper_target_acceleration_periods[channel] == 0) {
            *(ocrxa_registers[channel]) = stepper_target_pulse_periods[channel];
        } else {
            int16_t distance_to_edge = min(
                abs(stepper_positions[channel] - stepper_targets[channel]),
                abs(stepper_positions[channel] - stepper_target_initial_positions[channel])
            );
            uint16_t speed_factor = 8 * (distance_to_edge + 1) / stepper_target_acceleration_periods[channel];
            *(ocrxa_registers[channel]) = max(
                stepper_target_pulse_periods[channel],
                64 * 255 / speed_factor / speed_factor
            );
        }
    }

    // Stop motion if at target
    if (stepper_positions[channel] == stepper_targets[channel]) {
        *(ocrxb_registers[channel]) = 0;
    }
}
