#include "tri_ultrasound.h"
#include "pinout.h"

#include <Arduino.h>

static int pins[] = { PIN_ECHO_FRONT, PIN_ECHO_120, PIN_ECHO_240, 0 };

UltraSoundDetection UltraSounds;

void UltraSoundDetection::setupForInterrupt(char pin)
{
    *digitalPinToPCMSK(pin) |= bit(digitalPinToPCMSKbit(pin));
    PCIFR  |= bit(digitalPinToPCICRbit(pin));
    PCICR  |= bit(digitalPinToPCICRbit(pin));
}

ISR (PCINT1_vect)
{
  int newStatus;

  Serial.print("1 ffefeefeffe> ");

  for (int i = 0; i < NUMBER_ECHO; ++i) {
    newStatus = digitalRead(pins[i]);

    if ((newStatus == HIGH) && (UltraSounds.echoStatus[i] == UltraSoundDetection::Begin)) { // Rising
      UltraSounds.startTime[i] = micros();
      UltraSounds.echoStatus[i] = UltraSoundDetection::PulseStarted;
    }

    if ((newStatus == LOW) && (UltraSounds.echoStatus[i] == UltraSoundDetection::PulseStarted)) { // Falling
      UltraSounds.endTime[i] = micros();
      UltraSounds.echoStatus[i] = UltraSoundDetection::Done;
    }
  }
}

void UltraSoundDetection::begin()
{
  /* Configure the pins */
  for (int i = 0; i < NUMBER_ECHO; ++i) {
    pinMode(pins[i], INPUT);
    setupForInterrupt(pins[i]);
  }
}

void UltraSoundDetection::pulse(byte modules)
{
  for (int i = 0; i < NUMBER_ECHO; ++i) {
    
  Serial.println("Printing A0");
  digitalWrite(A0, HIGH);
  Serial.println("Printing A1");
  digitalWrite(A1, HIGH);
  Serial.println("Printing A5");
  digitalWrite(A5, HIGH);

    /* Skip unselected modules */
    if (!(_BV(i) & modules))
      continue;
      
    PCICR  = 0;
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(pins[i], LOW);
    pinMode(pins[i], INPUT);
    setupForInterrupt(pins[i]);
    echoStatus[i] = Begin;
  }
}

int UltraSoundDetection::read(byte module)
{
  for (int i = 0; i < NUMBER_ECHO; ++i) {
    if (_BV(i) & module) {
      if (echoStatus[i] != Done)
        return -1;
      return (endTime[i] - startTime[i]) * SPEED_OF_SOUND / 2 / 1000;
    }
  }
  return -1;
}
