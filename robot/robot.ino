#include "trajectory_control.h"
#include "pinout.h"
#include "servo_resources.h"

#define EPSILON        40 // to be able to release a buoy
#define FRONT_MARGIN   20 // to be able to release a buoy
#define REAR_MARGIN    (-40) // margin for hitting the lighthouse
#define SAFE_RELEASE_DISTANCE 200
#define SAFE_EDGE_PORT_X 700

static unsigned long gameStartTime = 0;
static TrajectoryControl::TeamSide teamSide = TrajectoryControl::TeamSide::Right;

void setup()
{
  Serial.begin(9600);
  Trajectory.begin(ROBOT_HALF_SIDE, (GREEN_CHANNEL_Y + RED_CHANNEL_Y) / 2, 0, teamSide, timerCallback);
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

  gameStartTime = millis();
}

void initializeServoSystems()
{
  Servos.begin();
  delay(500);
  SERVO_FLAG_LOWER();
  SERVO_FLIPPER_LEFT_CLOSE();
  SERVO_FLIPPER_RIGHT_CLOSE();
}

void initialFixedRoutine()
{
  // At rest in the middle of the port
  Trajectory.moveTo(SAFE_EDGE_PORT_X, (GREEN_CHANNEL_Y + RED_CHANNEL_Y) / 2, DONTCARE);
  // At the exist of the port
  Trajectory.moveTo(UNCHANGED, 470, 180);
  // Ready to capture the two northern buoys
  correctedServoFlipperLeftOpen();
  correctedServoFlipperRightOpen();
  Trajectory.moveTo(ROBOT_HALF_SIDE + EPSILON, UNCHANGED, DONTCARE);
  correctedServoFlipperRightClose();
  // One green buoy validated, the red one captured
  Trajectory.translate(-SAFE_RELEASE_DISTANCE);
  correctedServoFlipperLeftClose();
  // Green buoy now safe
  Trajectory.rotate(-125);
  Trajectory.translateWithoutCheck(-270);
  Trajectory.rotate(35);
  // Rear of the robot towards lighthouse
  Trajectory.translateWithoutCheck(-121);
  // Lighthouse now activated
  Trajectory.translate(121);
  // Now away from lighthouse
  Trajectory.moveTo(SAFE_EDGE_PORT_X + 100, 1160, 180);
  // Ready to deposit the two red buoys and capture another green one
  correctedServoFlipperLeftOpen();
  correctedServoFlipperRightOpen();
  Trajectory.moveTo(ROBOT_HALF_SIDE + EPSILON + 150, UNCHANGED, DONTCARE);
  correctedServoFlipperLeftClose();
  // The two red buoys are now validated
  Trajectory.translate(-390);
  correctedServoFlipperRightClose();
  // Now done with the red buoys
  Trajectory.moveTo(UNCHANGED, 470, 180);
  correctedServoFlipperLeftOpen();
  Trajectory.moveTo(ROBOT_HALF_SIDE + EPSILON + 150, UNCHANGED, DONTCARE);
  Trajectory.translate(-SAFE_RELEASE_DISTANCE + 100);
  correctedServoFlipperLeftClose();
  // Green buoy saved

  Trajectory.doNothing();
}

bool timerCallback()
{
  unsigned long elapsedTime = millis() - gameStartTime;

  if (elapsedTime > 95000) {
    SERVO_FLAG_RAISE();
  }

  // One second of margin of error
  return (elapsedTime < 99000);
}

void correctedServoFlipperLeftOpen() 
{
  if (teamSide == TrajectoryControl::TeamSide::Left) {
    SERVO_FLIPPER_LEFT_OPEN();
  } else {
    SERVO_FLIPPER_RIGHT_OPEN();
  }
}

void correctedServoFlipperRightOpen() 
{
  if (teamSide == TrajectoryControl::TeamSide::Left) {
    SERVO_FLIPPER_RIGHT_OPEN();
  } else {
    SERVO_FLIPPER_LEFT_OPEN();
  }
}

void correctedServoFlipperLeftClose() 
{
  if (teamSide == TrajectoryControl::TeamSide::Left) {
    SERVO_FLIPPER_LEFT_CLOSE();
  } else {
    SERVO_FLIPPER_RIGHT_CLOSE();
  }
}

void correctedServoFlipperRightClose() 
{
  if (teamSide == TrajectoryControl::TeamSide::Left) {
    SERVO_FLIPPER_RIGHT_CLOSE();
  } else {
    SERVO_FLIPPER_LEFT_CLOSE();
  }
}
