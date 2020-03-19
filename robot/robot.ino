#include "trajectory_control.h"
#include "pinout.h"

void setup()
{
  Serial.begin(9600);
  Trajectory.begin(ROBOT_SIDE / 2, (GREEN_CHANNEL_Y + RED_CHANNEL_Y) / 2, 0);

  // Wait for start signal
  pinMode(PIN_STARTER, INPUT_PULLUP);
  delay(500);
  while (digitalRead(PIN_STARTER) == LOW);
  
  Trajectory.moveForward(550);
  Trajectory.moveTo(UNCHANGED, GREEN_CHANNEL_Y + CUP_ENTRANCE_OFFSET_CENTER, 180);
  Trajectory.moveTo(ROBOT_SIDE / 2 + 30, UNCHANGED);
  Trajectory.moveForward(-520);
  Trajectory.moveTo(UNCHANGED, RED_CHANNEL_Y - CUP_ENTRANCE_OFFSET_CENTER, 180);
  Trajectory.moveTo(ROBOT_SIDE / 2 + 30, UNCHANGED);
  Trajectory.moveForward(-520);
}

void loop()
{
  // Nothing to loop
}
