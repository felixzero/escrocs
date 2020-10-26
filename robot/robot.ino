#include "trajectory_control.h"
#include "pinout.h"
#include "servo_resources.h"

#define EPSILON        40 // to be able to release a buoy
#define FRONT_MARGIN   20 // to be able to release a buoy
#define REAR_MARGIN    (-40) // margin for hitting the lighthouse
#define SAFE_RELEASE_DISTANCE 200
#define SAFE_EDGE_PORT_X 700

void setup()
{
  Serial.begin(9600);
  Trajectory.begin(ROBOT_HALF_SIDE, (GREEN_CHANNEL_Y + RED_CHANNEL_Y) / 2, 0, TrajectoryControl::TeamSide::Left);
  initializeServoSystems();

  waitForStartSignal();
  initialFixedRoutine();
}

void loop()
{
  // Nothing to loop
}

void waitForStartSignal()
{
  pinMode(PIN_STARTER, INPUT_PULLUP);
  delay(500);
  while (digitalRead(PIN_STARTER) == LOW);
}

void initializeServoSystems()
{
  Servos.begin();
  delay(500);
  SERVO_FLAG_LOWER();
  delay(500);
  SERVO_FLIPPER_LEFT_CLOSE();
  delay(500);
  SERVO_FLIPPER_RIGHT_CLOSE();
  delay(500);
  SERVO_ARM_RETRACT();
  delay(500);
}

void initialFixedRoutine()
{
  // At rest in the middle of the port
  Trajectory.moveTo(SAFE_EDGE_PORT_X, (GREEN_CHANNEL_Y + RED_CHANNEL_Y) / 2, DONTCARE);
  // At the exist of the port
  Trajectory.moveTo(UNCHANGED, 455, 180);
  // Ready to capture the two northern buoys
  SERVO_FLIPPER_LEFT_OPEN();
  SERVO_FLIPPER_RIGHT_OPEN();
  Trajectory.moveTo(ROBOT_HALF_SIDE + EPSILON, UNCHANGED, DONTCARE);
  SERVO_FLIPPER_RIGHT_CLOSE();
  // One green buoy validated, the red one captured
  Trajectory.translate(-SAFE_RELEASE_DISTANCE);
  SERVO_FLIPPER_LEFT_CLOSE();
  // Green buoy now safe
  Trajectory.rotate(-125);
  Trajectory.translate(-270);
  Trajectory.rotate(35);
  // Rear of the robot towards lighthouse
  Trajectory.translateWithoutCheck(-106);
  // Lighthouse now activated
  Trajectory.translate(106);
  // Now away from lighthouse
  Trajectory.moveTo(SAFE_EDGE_PORT_X + 100, 1140, 180);
  // Ready to deposit the two red buoys and capture another green one
  SERVO_FLIPPER_LEFT_OPEN();
  SERVO_FLIPPER_RIGHT_OPEN();
  Trajectory.moveTo(ROBOT_HALF_SIDE + EPSILON + 50, UNCHANGED, DONTCARE);
  SERVO_FLIPPER_LEFT_CLOSE();
  // The two red buoys are not validated
  Trajectory.translate(-290);
  SERVO_FLIPPER_RIGHT_CLOSE();
  // Now done with the red buoys
  Trajectory.moveTo(UNCHANGED, 1450, DONTCARE);
  // Now approaching the windsocks
  Trajectory.translateWithoutCheck(FIELD_Y - ROBOT_HALF_SIDE - 50 - 1450);
  Trajectory.rotate(90);
  Trajectory.translateWithoutCheck(280);
  // Now doing rear motion
  SERVO_ARM_DEPLOY();
  Trajectory.translateWithoutCheck(-600);
  SERVO_ARM_RETRACT();
}
