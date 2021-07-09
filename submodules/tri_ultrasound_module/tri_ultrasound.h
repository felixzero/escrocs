#pragma once

#include <Arduino.h>

#define ULTRASOUND_NONE   _BV(0)
#define ULTRASOUND_FRONT  _BV(1)
#define ULTRASOUND_RIGHT  _BV(2)
#define ULTRASOUND_LEFT   _BV(3)

#define PIN_ECHO_FRONT    A0
#define PIN_ECHO_120      A1
#define PIN_ECHO_240      A5

#define NUMBER_ECHO 3

#define MIN_PULSE_DELAY 80 // ms

#define SPEED_OF_SOUND 343

class UltraSoundDetection
{ 
public:
  enum Status { Begin = 0, PulseStarted = 1, Done = 2 };

  // Initialize modules.
  void begin();

  // Send ultrasound pulse to a specific set of emitters.
  void pulse(byte modules);

  // Returns the distance in mm to the obstacle, read by a specific module.
  // Returns -1 if no data is available.
  int read(byte module);

private:
  void setupForInterrupt(char pin);
  
public: // but actually private, only ISR must call this
  Status echoStatus[NUMBER_ECHO] = { Begin };
  unsigned long startTime[NUMBER_ECHO];
  unsigned long endTime[NUMBER_ECHO];
};

extern UltraSoundDetection UltraSounds;
