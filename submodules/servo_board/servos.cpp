#include <SPI.h>

#include "servos.h"
#include "pinout.h"

#define GSCLK_PERIOD 77
#define PWM_PERIOD 40000 // 50 Hz with 16 MHz clock and /16 prescaler
#define SERVO_MIN_WIDTH 204 // 1 ms
#define SERVO_MAX_WIDTH 410 // 2 ms

ServoBoard Servos;

void ServoBoard::begin()
{
  // Set up pins
  pinMode(PIN_SERVOS_BLANK, OUTPUT);
  pinMode(PIN_SERVOS_XLAT, OUTPUT);
  digitalWrite(PIN_SERVOS_BLANK, HIGH);

  // Stop interrupts
  cli();
  
  // Set up timer for GSCLK oscillation
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20); // Set up fast PWM mode on pin 3
  TCCR2B = _BV(WGM22); // Setup PWM to OCR2A
  OCR2B = 1; // Send only quick pulses
  OCR2A = GSCLK_PERIOD; // Set up period of GSCLK
  
  // Set up timer for BLANK timer (ISR)
  TCCR1A = 0; // No output on pins
  TCCR1B = _BV(WGM13) | _BV(WGM12); // CTC to ICR1
  ICR1 = PWM_PERIOD;
  
  // Start timers
  TCCR2B |= _BV(CS20); // No prescaler
  TCCR1B |= _BV(CS11); // /8 prescaler
  
  // Enable interrupts
  sei();
}

void ServoBoard::write(byte channel, byte value)
{
  channelValues[channel] = value;
  resendSerialData();  
}

void ServoBoard::resendSerialData()
{
  // Send servo data values by packets of 12 bits
  for (byte i = 0; i < NUMBER_OF_SERVO_CHANNELS / 2; ++i) {
    word first = servoTo12bits(channelValues[2*i]);
    word second = servoTo12bits(channelValues[2*i + 1]);
    
    SPI.transfer((first & 0x0FF0) >> 4);
    SPI.transfer((first & 0x000F) << 8 | (second & 0x0F00) >> 8);
    SPI.transfer(second & 0x00FF);
  }
  
  // Activate latch
  digitalWrite(PIN_SERVOS_XLAT, HIGH);
  digitalWrite(PIN_SERVOS_XLAT, LOW);
}

word ServoBoard::servoTo12bits(byte servoValue)
{
  return 4095 - SERVO_MIN_WIDTH - (word)servoValue * (SERVO_MAX_WIDTH - SERVO_MIN_WIDTH) / 256;
}

// ISR for handling periodic blank pulses
ISR (TIMER1_OVF_vect)
{
  // Pulse the blanking line
  digitalWrite(PIN_SERVOS_BLANK, HIGH);
  digitalWrite(PIN_SERVOS_BLANK, LOW);
}



