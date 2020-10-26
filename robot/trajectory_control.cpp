#include "trajectory_control.h"
#include "motor.h"
#include "ultrasound.h"

#include <math.h>
#include <Arduino.h>

TrajectoryControl Trajectory;

void TrajectoryControl::begin(int initialX, int initialY, int initialTheta, TeamSide side)
{
  _currentX = initialX;
  _currentY = initialY;
  _currentTheta = initialTheta;
  _side = side;

  Motor.begin();
  UltraSounds.begin();
}

void TrajectoryControl::moveTo(int destinationX, int destinationY, int destinationTheta)
{  
  destinationX = (destinationX == UNCHANGED) ? _currentX : destinationX;
  destinationY = (destinationY == UNCHANGED) ? _currentY : destinationY;
  destinationTheta = (destinationTheta == UNCHANGED) ? _currentTheta : destinationTheta;

  int trajectoryAngle = DEG_ATAN2(destinationY - _currentY, destinationX - _currentX);
  int angleDifference = MODULO_ANGLE(trajectoryAngle - _currentTheta);  
  rotate((angleDifference <= 180) ? angleDifference : angleDifference - 360);

  int distance = INT_DISTANCE(destinationX - _currentX, destinationY - _currentY);
  translate(distance);

  if (destinationTheta != DONTCARE) {
    angleDifference = MODULO_ANGLE(destinationTheta - _currentTheta);
    rotate((angleDifference <= 180) ? angleDifference : angleDifference - 360);
  }
}

void TrajectoryControl::rotate(int angle)
{
  int teamSideCorrectedAngle = (_side == TeamSide::Left ? angle : -angle);
  Motor.setTarget(MotorControl::Turning, teamSideCorrectedAngle);

  controlLoop(CollisionCheckType::None);

  _currentTheta += angle;
  _currentTheta = MODULO_ANGLE(_currentTheta);
}

void TrajectoryControl::translate(int distance)
{
  Motor.setTarget(MotorControl::Forward, distance);

  controlLoop(distance >= 0 ? CollisionCheckType::Front : CollisionCheckType::Rear);

  _currentX += DEG_COS(distance, _currentTheta);
  _currentY += DEG_SIN(distance, _currentTheta);
}

void TrajectoryControl::translateWithoutCheck(int distance)
{
  Motor.setTarget(MotorControl::Forward, distance);

  controlLoop(CollisionCheckType::None);

  _currentX += DEG_COS(distance, _currentTheta);
  _currentY += DEG_SIN(distance, _currentTheta);
}

void TrajectoryControl::controlLoop(CollisionCheckType checkForCollisions)
{
  int ultrasoundLeft, ultrasoundRight;
  setupUltrasoundResources(checkForCollisions, ultrasoundLeft, ultrasoundRight);
  
  while (true) {
    unsigned long pulseTime = millis();
    
    UltraSounds.pulse(ultrasoundLeft | ultrasoundRight);

    while ((millis() - pulseTime) < MIN_PULSE_DELAY) {
      Motor.pollRegulation();
      if (Motor.isIdle()) {
        return;
      }

      if ((checkForCollisions != CollisionCheckType::None) && (!IS_DISTANCE_SAFE(UltraSounds.read(ultrasoundLeft)) || !IS_DISTANCE_SAFE(UltraSounds.read(ultrasoundRight)))) {
          Motor.stopMotion();
          waitForFreePath();
          Motor.resumeMotion();
        }
    }
  }
}

void TrajectoryControl::setupUltrasoundResources(CollisionCheckType checkForCollisions, int& left, int& right)
{
  switch(checkForCollisions) {
  case CollisionCheckType::Front:
    left = ULTRASOUND_FL;
    right = ULTRASOUND_FR;
  case CollisionCheckType::Rear:
    left = ULTRASOUND_RL;
    right = ULTRASOUND_RR;
  default:
    left = ULTRASOUND_NONE;
    right = ULTRASOUND_NONE;
  }
}

void TrajectoryControl::waitForFreePath()
{
  while (true) {
    UltraSounds.pulse(ULTRASOUND_FRONT);
    delay(MIN_PULSE_DELAY);
    if (IS_DISTANCE_SAFE(UltraSounds.read(ULTRASOUND_FL)) && IS_DISTANCE_SAFE(UltraSounds.read(ULTRASOUND_FR))) {
      break;
    }
  }
}

inline int TrajectoryControl::DEG_ATAN2(int y, int x)
{
  return (int)(360 + atan2((double)y, (double)x) * 45 / atan(1.0));
}

inline int TrajectoryControl::DEG_COS(int r, int theta)
{
  return (int)(r * cos((double)theta * atan(1.0) / 45));
}

inline int TrajectoryControl::DEG_SIN(int r, int theta)
{
  return (int)(r * sin((double)theta * atan(1.0) / 45));
}


inline int TrajectoryControl::MODULO_ANGLE(int theta)
{
  while (theta < 0) { theta += 360; }
  while (theta > 360) { theta -= 360; }
  return theta;
}

inline int TrajectoryControl::INT_DISTANCE(int x, int y)
{
  return (int)(sqrt(((double)x * x + (double)y * y)));
}

inline bool TrajectoryControl::IS_DISTANCE_SAFE(int distance)
{
  return (distance < 0) || (distance >= DANGER_DISTANCE);
}
