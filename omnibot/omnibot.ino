#include <Wire.h>

#include "omni_trajectory_control.h"
#include "pinout.h"

static unsigned long gameStartTime = 0;
static TrajectoryControl::TeamSide teamSide = TrajectoryControl::TeamSide::Left;

void setup() {
  Serial.begin(9600);

  Trajectory.begin(ROBOT_HALF_SIDE, (GREEN_CHANNEL_Y + RED_CHANNEL_Y) / 2, 0, teamSide, timerCallback);

  waitForStartSignal();
  raiseWindBuoy();
}

void loop() {
  // put your main code here, to run repeatedly:
}

void waitForStartSignal()
{
  pinMode(PIN_STARTER, INPUT_PULLUP);
  delay(500);
  while (digitalRead(PIN_STARTER) == LOW);

  gameStartTime = millis();
}

bool timerCallback()
{
  unsigned long elapsedTime = millis() - gameStartTime;

  // One second of margin of error
  return (elapsedTime < 99000);
}

void raiseWindBuoy()
{
  //goto south wall

  //Push

  //End
  Trajectory.doNothing();

  //  // At rest in the middle of the port
  //  Trajectory.moveTo(SAFE_EDGE_PORT_X, (GREEN_CHANNEL_Y + RED_CHANNEL_Y) / 2, DONTCARE);
  //  // At the exist of the port
  //  Trajectory.moveTo(UNCHANGED, 470, 180);
  //  // Ready to capture the two northern buoys
  //  correctedServoFlipperLeftOpen();
  //  correctedServoFlipperRightOpen();
  //  Trajectory.moveTo(ROBOT_HALF_SIDE + EPSILON, UNCHANGED, DONTCARE);
  //  correctedServoFlipperRightClose();
  //  // One green buoy validated, the red one captured
  //  Trajectory.translate(-SAFE_RELEASE_DISTANCE);
  //  correctedServoFlipperLeftClose();
  //  // Green buoy now safe
  //  Trajectory.rotate(-125);
  //  Trajectory.translateWithoutCheck(-270);
  //  Trajectory.rotate(35);
  //  // Rear of the robot towards lighthouse
  //  Trajectory.translateWithoutCheck(-150);
  //  // Lighthouse now activated
  //  Trajectory.translate(125);
  //  // Now away from lighthouse
  //  Trajectory.moveTo(SAFE_EDGE_PORT_X + 100, 1160, 180);
  //  // Ready to deposit the two red buoys and capture another green one
  //  correctedServoFlipperLeftOpen();
  //  correctedServoFlipperRightOpen();
  //  Trajectory.moveTo(ROBOT_HALF_SIDE + EPSILON + 150, UNCHANGED, DONTCARE);
  //  correctedServoFlipperLeftClose();
  //  // The two red buoys are now validated
  //  Trajectory.translate(-390);
  //  correctedServoFlipperRightClose();
  //  // Now done with the red buoys
  //  Trajectory.moveTo(UNCHANGED, 470, 180);
  //  correctedServoFlipperLeftOpen();
  //  Trajectory.moveTo(ROBOT_HALF_SIDE + EPSILON + 150, UNCHANGED, DONTCARE);
  //  Trajectory.translate(-SAFE_RELEASE_DISTANCE);
  //  // Green buoy saved
  //  // x = 510; y = 470; theta = 180
  //  Trajectory.moveTo(1050, 430, DONTCARE);
  //  correctedServoFlipperLeftClose();
  //  Trajectory.rotate(180);
  //  correctedServoFlipperLeftOpen();
  //  Trajectory.moveTo(ROBOT_HALF_SIDE + EPSILON + 200, 410, DONTCARE);
  //  Trajectory.translate(-SAFE_RELEASE_DISTANCE + 100);
  //  correctedServoFlipperLeftClose();
  //
  //  Trajectory.doNothing();
}
