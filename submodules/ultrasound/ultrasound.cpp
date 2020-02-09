#include "ultrasound.h"
#include "pinout.h"

#include <Arduino.h>

void _setup_for_interrupt(char pin)
{
    *digitalPinToPCMSK(pin) |= bit(digitalPinToPCMSKbit(pin));
    PCIFR  |= bit(digitalPinToPCICRbit(pin));
    PCICR  |= bit(digitalPinToPCICRbit(pin));
}

static char pins_echo[] = { PIN_ECHO_FL, PIN_ECHO_FR, PIN_ECHO_RL, PIN_ECHO_RR };
#define NUMBER_ECHO sizeof(pins_echo)

#define ECHO_STATUS_BEGIN 0
#define ECHO_STATUS_PULSE_STARTED 1
#define ECHO_STATUS_DONE 2
static byte echo_status[NUMBER_ECHO] = { ECHO_STATUS_BEGIN };

static unsigned long start_time[NUMBER_ECHO];
static unsigned long end_time[NUMBER_ECHO];

ISR (PCINT2_vect)
{
  int new_status, i;

  for (i = 0; i < NUMBER_ECHO; ++i) {
    new_status = digitalRead(pins_echo[i]);

    if ((new_status == HIGH) && (echo_status[i] == ECHO_STATUS_BEGIN)) { // Rising
      start_time[i] = micros();
      echo_status[i] = ECHO_STATUS_PULSE_STARTED;
    }

    if ((new_status == LOW) && (echo_status[i] == ECHO_STATUS_PULSE_STARTED)) { // Falling
      end_time[i] = micros();
      echo_status[i] = ECHO_STATUS_DONE;
    }
  }
}

void ultrasound_init()
{
  int i;

  /* Configure the pins */
  for (i = 0; i < NUMBER_ECHO; ++i) {
    pinMode(pins_echo[i], INPUT);
    _setup_for_interrupt(pins_echo[i]);
  }
}

void ultrasound_pulse(char module_selection_flag)
{
  int i;

  for (i = 0; i < NUMBER_ECHO; ++i) {
    /* Skip unselected modules */
    if (!((1 << i) & module_selection_flag))
      continue;

    pinMode(pins_echo[i], OUTPUT);
    digitalWrite(pins_echo[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(pins_echo[i], LOW);
    pinMode(pins_echo[i], INPUT);
    echo_status[i] = ECHO_STATUS_BEGIN;
  }
}

int ultrasound_read(char module)
{
  if (echo_status[module] != ECHO_STATUS_DONE)
    return -1;
  return (end_time[module] - start_time[module]) * 343 / 2000;
}
