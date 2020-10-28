#include <Wire.h>

#include "motor.h"

void setup()
{
  Serial.begin(9600);
  Motor.begin();
  delay(1000);
}

void debugTurning()
{
  Motor.setTarget(MotorControl::Turning, -180);
  while (!Motor.isIdle())
    Motor.pollRegulation();
  delay(1000);
}

void debug_forward()
{
  Motor.setTarget(MotorControl::Forward, 800);
  while (!Motor.isIdle())
    Motor.pollRegulation();
  delay(1000);

  Motor.setTarget(MotorControl::Forward, -800);
  while (!Motor.isIdle())
    Motor.pollRegulation();
  delay(1000);
}

void loop()
{
   debugTurning();
  // debugForward();
}
