#include <SPI.h>

#include "servos.h"
#include "pinout.h"

// All values with /8 prescaler: clock at 2MHz
#define GSCLK_PERIOD 10 // 5us
#define PWM_PERIOD 20480 // 48.8Hz
#define SERVO_MIN_WIDTH 200 // 1 ms
#define SERVO_MAX_WIDTH 400 // 2 ms
#define NO_PULSE_VALUE (byte(12) - 1)

ServoBoard Servos;

void ServoBoard::begin()
{
  // Set up pins
  pinMode(PIN_SERVOS_XLAT, OUTPUT);
  pinMode(PIN_SERVOS_GSCLK, OUTPUT);
  pinMode(PIN_SERVOS_BLANK, OUTPUT);

  // Stop interrupts
  cli();
  
  // Set up timer for GSCLK oscillation
  // Fast PWM to OCR2A, clear output on pin 3
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22);
  OCR2B = 1; // Send only quick pulses
  OCR2A = GSCLK_PERIOD - 1; // Set up period of GSCLK
  
  // Set up timer for BLANK timer (ISR)
  // PWM to ICR1, clear output on pin 9
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM13);
  OCR1A = GSCLK_PERIOD; // Pulse duration
  ICR1 = PWM_PERIOD - 1;
  
  // Start timers, with /8 prescalers
  TCCR2B |= _BV(CS21);
  TCCR1B |= _BV(CS11);
  
  // Enable interrupts
  sei();

  // Set all channels to neutral by default 
  for (byte i = 0; i < NUMBER_OF_SERVO_CHANNELS; ++i)
    channelValues[i] = NO_PULSE_VALUE;
  
  SPI.begin();
  resendSerialData();
}

void ServoBoard::write(byte channel, int value)
{
  channelValues[channel] = NO_PULSE_VALUE - SERVO_MIN_WIDTH - (value + 127) * (SERVO_MAX_WIDTH - SERVO_MIN_WIDTH) / 256;
  resendSerialData();  
}

void ServoBoard::resendSerialData()
{  
  // Send servo data values by packets of 12 bits
  for (byte i = 0; i < NUMBER_OF_SERVO_CHANNELS / 2; ++i) {
    word first = channelValues[NUMBER_OF_SERVO_CHANNELS - 1 - 2*i];
    word second = channelValues[NUMBER_OF_SERVO_CHANNELS - 2 - 2*i];
    
    SPI.transfer((first & 0x0FF0) >> 4);
    SPI.transfer((first & 0x000F) << 4 | (second & 0x0F00) >> 8);
    SPI.transfer(second & 0x00FF);
  }
  
  // Activate latch
  digitalWrite(PIN_SERVOS_XLAT, HIGH);
  digitalWrite(PIN_SERVOS_XLAT, LOW);
}
