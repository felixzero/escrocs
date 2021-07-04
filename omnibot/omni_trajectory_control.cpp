#include "omni_trajectory_control.h"
#include "tri_ultrasound.h"

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
  lastDirectionConsign = 0xFFFF;

  UltraSounds.begin();
  Wire.begin();  
}

//consign is not used by motor board if command is READ_ENC.
long sendMotorCommand(int cmd, int channel, int consign)
{
  // Package CMD
  uint8_t command = cmd << 2;
  uint8_t msg = command & channel;

  //Transmit
  Wire.beginTransmission(MOTORS_ADDR);
  Wire.write(msg);
  Wire.write(consign);
  Wire.endTransmission();

  // Motor board always answers by writing the encoder value for the channel
  long enc = Wire.requestFrom(MOTORS_ADDR,ANSWERSIZE);
  return enc;
}

// Send consigns for all three motors
// TODO: check if the sequential sending doesnt cause stability problems...
void sendSpeedConsigns(int mot1, int mot2, int mot3)
{
  sendMotorCommand(CMD_WRITE_SPEED, 1, mot1);
  sendMotorCommand(CMD_WRITE_SPEED, 2, mot2);
  sendMotorCommand(CMD_WRITE_SPEED, 3, mot3);
}

// read encoders
long readEncoder(int enc)
{
  return sendMotorCommand(CMD_READ_ENC, enc, 0);
}


// Compute the contribution (and individual consign) for each motor
void TrajectoryControl::linearSpeedConsign(int speed, int direction)
{
  int mot1 = speed * cos(WHEEL_1_DIRECTION - direction);
  int mot2 = speed * cos(WHEEL_2_DIRECTION - direction);
  int mot3 = speed * cos(WHEEL_3_DIRECTION - direction);
  sendSpeedConsigns(mot1, mot2, mot3);
}

// In the general case (Both translation and rotation required), the contribution for each motor
// is the sum of the linear and angular contributions.
// NOTE: there should be a limit on SUM(linSpeed,angSpeed) to ensure no saturation occurs in the consign
void TrajectoryControl::generalSpeedConsign(int linSpeed, int direction, int angSpeed, Rotation rot)
{
   Rotation actualRot = rot;
  //TODO: check base side
  if(_side == TeamSide::Left)
  {
    if (rot == Rotation::Clockwise)
    {
      actualRot = Rotation::Counterclockwise;
    }
    else
    {
      actualRot = Rotation::Clockwise;
    }
  }
  int rotSpeed = angSpeed / ROBOT_RADIUS * (actualRot==Rotation::Clockwise?-1:1);
  int mot1 = linSpeed * cos(WHEEL_1_DIRECTION - direction) + rotSpeed;
  int mot2 = linSpeed * cos(WHEEL_2_DIRECTION - direction) + rotSpeed;
  int mot3 = linSpeed * cos(WHEEL_3_DIRECTION - direction) + rotSpeed;
  sendSpeedConsigns(mot1, mot2, mot3);
}

// To rotate, set all motors to the exact same speed in the same direction
// Angular velocity is the linear speed of the wheel * radius of the axle
void TrajectoryControl::rotate(int angularSpeed, Rotation rot, int duration)
{
  Rotation actualRot = rot;
  //TODO: check base side
  if(_side == TeamSide::Left)
  {
    if (rot == Rotation::Clockwise)
    {
      actualRot = Rotation::Counterclockwise;
    }
    else
    {
      actualRot = Rotation::Clockwise;
    }
  }
  int speedConsign = angularSpeed / ROBOT_RADIUS * (actualRot==Rotation::Clockwise?-1:1);
  
  //Motor.setTarget(MotorControl::Turning, teamSideCorrectedAngle);
  sendSpeedConsigns(speedConsign, speedConsign, speedConsign);
  lastDirectionConsign = 0xFFFF;
  controlLoop(CollisionCheckType::None, duration);
}

// Translate in the given direction for a fixed distance (well, time)
void TrajectoryControl::translate(int direction, int duration)
{
  //TODO: check speed consign
  linearSpeedConsign(50, direction);
  
  CollisionCheckType collision = CollisionCheckType::None;
  if(abs(direction) <= 45)
  {
    collision =  CollisionCheckType::Front;
  }
  else if(direction < 0 || direction > 180)
  {
    collision =  CollisionCheckType::Left;
  }
  else if(direction > 0 || direction <= 180)
  {
    collision =  CollisionCheckType::Right;
  }
  lastDirectionConsign = direction;
  controlLoop(collision, duration);
}

// Luigi wins!
void TrajectoryControl::doNothing()
{
    while (true) {
      checkTimerAndEndProgramIfNeeded();
      delay(MIN_PULSE_DELAY);
  }
}

// no fine control of speed profile (yet)
void TrajectoryControl::controlLoop(CollisionCheckType checkForCollisions, int duration)
{
  int sonar;
  setupUltrasoundResources(checkForCollisions, sonar);
  
  while (duration) {
    unsigned long pulseTime = millis();
    
    UltraSounds.pulse(sonar);

    while ((millis() - pulseTime) < MIN_PULSE_DELAY) {
      //pollRegulation();
      //if (isIdle()) {
      //  return;
      //}

      checkTimerAndEndProgramIfNeeded();

      if ((checkForCollisions != CollisionCheckType::None) && (!isDistanceSafe(UltraSounds.read(sonar)))) {
          stopMotion();
          waitForFreePath(sonar);
          resumeMotion();
      }

      delay(1);
    }
  }
}

//kill motors
void TrajectoryControl::stopMotion()
{
  sendSpeedConsigns(0, 0, 0);
}


//resume old direction
void TrajectoryControl::resumeMotion()
{
  //TODO: check speed consign
  linearSpeedConsign(50, lastDirectionConsign);
}

void TrajectoryControl::setupUltrasoundResources(CollisionCheckType checkForCollisions, int& sonar)
{
  switch(checkForCollisions) {
    case CollisionCheckType::Front:
      sonar = ULTRASOUND_FRONT;
    case CollisionCheckType::Right:
      sonar = ULTRASOUND_RIGHT;
    case CollisionCheckType::Left:
      sonar = ULTRASOUND_LEFT;
    default:
      sonar = ULTRASOUND_NONE;
  }
}

//uses the sonar computed when setting up the motion direction
void TrajectoryControl::waitForFreePath(int sonar)
{
  while (true) {
    checkTimerAndEndProgramIfNeeded();
    
    UltraSounds.pulse(sonar);
    delay(MIN_PULSE_DELAY);
    if (isDistanceSafe(UltraSounds.read(sonar))) {
      break;
    }
  }
}

void TrajectoryControl::checkTimerAndEndProgramIfNeeded()
{
  if (!_timerCallback()) {
    stopMotion();
    while (true) {
      delay(10000);
    }
  }
}

inline bool TrajectoryControl::isDistanceSafe(int distance)
{
  return (distance < 0) || (distance >= DANGER_DISTANCE);
}

//
//inline int TrajectoryControl::degAtan2(int y, int x)
//{
//  return (int)(360 + atan2((double)y, (double)x) * 45 / atan(1.0));
//}
//
//inline int TrajectoryControl::degCos(int r, int theta)
//{
//  return (int)(r * cos((double)theta * atan(1.0) / 45));
//}
//
//inline int TrajectoryControl::degSin(int r, int theta)
//{
//  return (int)(r * sin((double)theta * atan(1.0) / 45));
//}
//
//
//inline int TrajectoryControl::moduloAngle(int theta)
//{
//  while (theta < 0) { theta += 360; }
//  while (theta > 360) { theta -= 360; }
//  return theta;
//}
//
//inline int TrajectoryControl::intDistance(int x, int y)
//{
//  return (int)(sqrt(((double)x * x + (double)y * y)));
//}
//
