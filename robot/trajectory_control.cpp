#include "trajectory_control.h"
#include "motor.h"

#include <math.h>
#include <Arduino.h>

TrajectoryControl Trajectory;

void TrajectoryControl::begin(int initialX, int initialY, int initialTheta)
{
  _currentX = initialX;
  _currentY = initialY;
  _currentTheta = initialTheta;

  Motor.begin();
}

void TrajectoryControl::moveTo(int destinationX, int destinationY, int destinationTheta)
{  
  destinationX = (destinationX == UNCHANGED) ? _currentX : destinationX;
  destinationY = (destinationY == UNCHANGED) ? _currentY : destinationY;
  destinationTheta = (destinationTheta == UNCHANGED) ? _currentTheta : destinationTheta;
  
  int angleDifference = (DEG_ATAN2(destinationY - _currentY, destinationX - _currentX) - _currentTheta + 360) % 360;  
  rotate((angleDifference <= 180) ? angleDifference : angleDifference - 360);

  int distance = INT_DISTANCE(destinationX - _currentX, destinationY - _currentY);
  moveForward(distance);

      
  return;

  if (destinationTheta != DONTCARE) {
    angleDifference = (destinationTheta - _currentTheta + 360) % 360;
    rotate((angleDifference <= 180) ? angleDifference : 360 - angleDifference);
  }
}

void TrajectoryControl::rotate(int angle)
{
  Motor.setTarget(MotorControl::Turning, -angle);

  while (!Motor.isIdle()) {
    Motor.pollRegulation();
    delay(20);
  }

  _currentTheta += angle;
  _currentTheta %= 360;
}

void TrajectoryControl::moveForward(int distance)
{
  Motor.setTarget(MotorControl::Forward, distance);

  while (!Motor.isIdle()) {
    Motor.pollRegulation();
    delay(20);
  }

  _currentX += DEG_COS(distance, _currentTheta);
  _currentY += DEG_SIN(distance, _currentTheta);
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

inline int TrajectoryControl::INT_DISTANCE(int x, int y)
{
  return (int)(sqrt(((double)x * x + (double)y * y)));
}
