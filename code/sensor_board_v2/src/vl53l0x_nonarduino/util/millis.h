#ifndef MILLIS_H
#define MILLIS_H   1

// Millisecond timer, inspired by the Arduino linrary
// Uses Timer0 interrupts

//--------------------------------------------------
// Macros to manage fractional ticks
//--------------------------------------------------
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (F_CPU / 1000L) )
#define T0_PRESCALE 1024
#define T0_RELOAD    244/2
#define MICROSECONDS_PER_TIMER0_OVERFLOW ( clockCyclesToMicroseconds(T0_PRESCALE*(T0_RELOAD+1L)) )
// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC       (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
// the fractional number of microseconds per timer0 overflow.
#define MILLIS_INC_FRACT (MICROSECONDS_PER_TIMER0_OVERFLOW % 1000)

//--------------------------------------------------
// Public functions
//--------------------------------------------------
// sets-up Timer0 (prescale, reload, interrupts)
void initMillis();

//Return number of milliseconds since startup
uint32_t millis();

#endif