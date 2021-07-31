#pragma once

#include <Arduino.h>

#define ULTRASOUND_0    _BV(0)
#define ULTRASOUND_60   _BV(1)
#define ULTRASOUND_120  _BV(2)
#define ULTRASOUND_180  _BV(3)
#define ULTRASOUND_240  _BV(4)
#define ULTRASOUND_300  _BV(5)
#define ULTRASOUND_FRONT (ULTRASOUND_0   | ULTRASOUND_60  | ULTRASOUND_300)
#define ULTRASOUND_REAR  (ULTRASOUND_180 | ULTRASOUND_120 | ULTRASOUND_240)

#define NUMBER_ECHO 6

#define MIN_PULSE_DELAY 80 // ms

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
