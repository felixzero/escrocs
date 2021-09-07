#include <Arduino.h>

#include "motor.h"
#include "motor_isr.h"

#define _C1A_REG portd_values
#define _C1A_FLAG 0b00100000u
#define _C1B_REG portd_values
#define _C1B_FLAG 0b01000000u

#define _C2A_REG portb_values
#define _C2A_FLAG 0b00000010u
#define _C2B_REG portb_values
#define _C2B_FLAG 0b00000100u

#define _C3A_REG portd_values
#define _C3A_FLAG 0b00001000u
#define _C3B_REG portb_values
#define _C3B_FLAG 0b00000001u

void init_motors()
{
    noInterrupts();

    // Configure timer 1; no output, CTC to ICR1 (set to 255), prescaler /64 (1.0 kHz)
    TCCR1A = 0;
    TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11) | _BV(CS10);
    // Enable COMPA interrupt
    TIMSK1 = _BV(OCIE1A);
    ICR1 = 255;

    interrupts();

    pinMode(3, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
}

void write_motor_speed(float speed1, float speed2, float speed3)
{
    uint8_t timings[3];
    timings[0] = (uint8_t)(255 - min(1.0, fabs(speed1)) * 254);
    timings[1] = (uint8_t)(255 - min(1.0, fabs(speed2)) * 254);
    timings[2] = (uint8_t)(255 - min(1.0, fabs(speed3)) * 254);

    volatile uint8_t *regs[3];
    regs[0] = (speed1 > 0.0) ? _C1A_REG : _C1B_REG;
    regs[1] = (speed2 > 0.0) ? _C2A_REG : _C2B_REG;
    regs[2] = (speed3 > 0.0) ? _C3A_REG : _C3B_REG;

    uint8_t flags[3];
    flags[0] = (speed1 > 0.0) ? _C1A_FLAG : _C1B_FLAG;
    flags[1] = (speed2 > 0.0) ? _C2A_FLAG : _C2B_FLAG;
    flags[2] = (speed3 > 0.0) ? _C3A_FLAG : _C3B_FLAG;

    // Values at t=0: everything is high
    channel_values[0] = 0;
    for (int i = 0; i < 4; ++i) {
        portb_values[i] = 0xFF;
        portd_values[i] = 0xFF;
    }

    for (uint8_t j = 1; j < 4; ++j) {
        uint8_t min_timing = min(timings[0], min(timings[1], timings[2]));
        channel_values[j] = min_timing;
        for (uint8_t i = 0; i < 3; ++i) {
            if (timings[i] == min_timing) {
                for (uint8_t k = j; k < 4; ++k) {
                    regs[i][k] &= ~flags[i];
                }
                timings[i] = 255;
            }
        }
    }
}
