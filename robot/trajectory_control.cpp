#include "trajectory_control.h"
#include "motor.h"
#include "ultrasound.h"

#include <math.h>
#include <Arduino.h>

TrajectoryControl Trajectory;

void TrajectoryControl::begin(int initialX, int initialY, int initialTheta, TeamSide side, timerCallback_t timerCallback)
{
  _currentX = initialX;
  _currentY = initialY;
  _currentTheta = initialTheta;
  _side = side;
  _timerCallback = timerCallback;

  Motor.begin();
  UltraSounds.begin();
}

void TrajectoryControl::moveTo(int destinationX, int destinationY, int destinationTheta)
{  
  destinationX = (destinationX == UNCHANGED) ? _currentX : destinationX;
  destinationY = (destinationY == UNCHANGED) ? _currentY : destinationY;
  destinationTheta = (destinationTheta == UNCHANGED) ? _currentTheta : destinationTheta;

  int trajectoryAngle = degAtan2(destinationY - _currentY, destinationX - _currentX);
  int angleDifference = moduloAngle(trajectoryAngle - _currentTheta);  
  rotate((angleDifference <= 180) ? angleDifference : angleDifference - 360);

  int distance = intDistance(destinationX - _currentX, destinationY - _currentY);
  translate(distance);

  if (destinationTheta != DONTCARE) {
    angleDifference = moduloAngle(destinationTheta - _currentTheta);
    rotate((angleDifference <= 180) ? angleDifference : angleDifference - 360);
  }
}

void TrajectoryControl::rotate(int angle)
{
  int teamSideCorrectedAngle = (_side == TeamSide::Left ? angle : -angle);
  Motor.setTarget(MotorControl::Turning, teamSideCorrectedAngle);

  controlLoop(CollisionCheckType::None);

  _currentTheta += angle;
  _currentTheta = moduloAngle(_currentTheta);
}

void TrajectoryControl::translate(int distance)
{
  Motor.setTarget(MotorControl::Forward, distance);

  controlLoop(distance >= 0 ? CollisionCheckType::Front : CollisionCheckType::Rear);

  _currentX += degCos(distance, _currentTheta);
  _currentY += degSin(distance, _currentTheta);
}

void TrajectoryControl::translateWithoutCheck(int distance)
{
  Motor.setTarget(MotorControl::Forward, distance);

  controlLoop(CollisionCheckType::None);

  _currentX += degCos(distance, _currentTheta);
  _currentY += degSin(distance, _currentTheta);
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

      checkTimerAndEndProgramIfNeeded();

      if ((checkForCollisions != CollisionCheckType::None) && (!isDistanceSafe(UltraSounds.read(ultrasoundLeft)) || !isDistanceSafe(UltraSounds.read(ultrasoundRight)))) {
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
    checkTimerAndEndProgramIfNeeded();
    
    UltraSounds.pulse(ULTRASOUND_FRONT);
    delay(MIN_PULSE_DELAY);
    if (isDistanceSafe(UltraSounds.read(ULTRASOUND_FL)) && isDistanceSafe(UltraSounds.read(ULTRASOUND_FR))) {
      break;
    }
  }
}

void TrajectoryControl::checkTimerAndEndProgramIfNeeded()
{
  if (!_timerCallback()) {
    Motor.stopMotion();
    while (true) {
      delay(10000);
    }
  }
}

inline int TrajectoryControl::degAtan2(int y, int x)
{
  return (int)(360 + atan2((double)y, (double)x) * 45 / atan(1.0));
}

inline int TrajectoryControl::degCos(int r, int theta)
{
  return (int)(r * cos((double)theta * atan(1.0) / 45));
}

inline int TrajectoryControl::degSin(int r, int theta)
{
  return (int)(r * sin((double)theta * atan(1.0) / 45));
}


inline int TrajectoryControl::moduloAngle(int theta)
{
  while (theta < 0) { theta += 360; }
  while (theta > 360) { theta -= 360; }
  return theta;
}

inline int TrajectoryControl::intDistance(int x, int y)
{
  return (int)(sqrt(((double)x * x + (double)y * y)));
}

inline bool TrajectoryControl::isDistanceSafe(int distance)
{
  return (distance < 0) || (distance >= DANGER_DISTANCE);
}
