#pragma once

#include <Arduino.h>

#define ULTRASOUND_FL _BV(0)
#define ULTRASOUND_FR _BV(1)
#define ULTRASOUND_RL _BV(2)
#define ULTRASOUND_RR _BV(3)
#define ULTRASOUND_FRONT (ULTRASOUND_FL | ULTRASOUND_FR)
#define ULTRASOUND_REAR (ULTRASOUND_RL | ULTRASOUND_RR)

#define NUMBER_ECHO 4

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
