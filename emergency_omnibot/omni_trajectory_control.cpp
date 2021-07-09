#include "omni_trajectory_control.h"

#include "motor.h"
#include "encoder.h"

#include <math.h>
#include <Arduino.h>

//#define DEBUG

#define DEFAULT_LINEAR_SPEED_CONSIGN -255
#define REDUCED_LINEAR_SPEED_CONSIGN DEFAULT_LINEAR_SPEED_CONSIGN * 3/4

TrajectoryControl Trajectory;

void TrajectoryControl::begin(int initialX, int initialY, int initialTheta, TeamSide side, timerCallback_t timerCallback)
{
  _currentX = initialX;
  _currentY = initialY;
  _currentTheta = initialTheta;
  _side = side;
  _timerCallback = timerCallback;
  lastDirectionConsign = 0xFFFF;

  Motor.begin();
  Encoder.begin();
}

// Send consigns for all three motors
// TODO: check if the sequential sending doesnt cause stability problems...
void TrajectoryControl::sendSpeedConsigns(int mot1, int mot2, int mot3)
{
  Motor.writeSpeed(0, mot1);
  Motor.writeSpeed(1, mot2);
  Motor.writeSpeed(2, mot3);
}

// Compute the contribution (and individual consign) for each motor
void TrajectoryControl::linearSpeedConsign(int speed, int direction)
{
  #ifdef DEBUG
  Serial.println("Linear speed consign:");
  Serial.println(speed);
  Serial.println(direction);
  #endif

  int mot1 = degCos(speed, WHEEL_1_DIRECTION - direction);
  int mot2 = degCos(speed, WHEEL_2_DIRECTION - direction);
  int mot3 = degCos(speed, WHEEL_3_DIRECTION - direction);

  #ifdef DEBUG
  Serial.print("Linear speed :");
  Serial.println(mot1);
  Serial.println(mot2);
  Serial.println(mot3);
  #endif
  
  sendSpeedConsigns(mot1, mot2, mot3);
}

// To rotate, set all motors to the exact same speed in the same direction
// Angular velocity is the linear speed of the wheel * radius of the axle
void TrajectoryControl::rotate(int angularSpeed, Rotation rot, int duration)
{
  Rotation actualRot = rot;
  if (_side == TeamSide::Left)
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
  int speedConsign = angularSpeed * (actualRot == Rotation::Clockwise ? 1 : -1);
  
  sendSpeedConsigns(speedConsign, speedConsign, speedConsign);
  lastDirectionConsign = 0xFFFF;
  controlLoop(CollisionCheckType::None, duration);
}

// Translate in the given direction for a fixed distance (well, time)
void TrajectoryControl::translate(int direction, int duration)
{
  //TODO: check speed consign
  linearSpeedConsign(DEFAULT_LINEAR_SPEED_CONSIGN, direction);
  currentLinearSpeed = DEFAULT_LINEAR_SPEED_CONSIGN;

  CollisionCheckType collision = CollisionCheckType::None;
  if (abs(direction) <= 45)
  {
    collision =  CollisionCheckType::Front;
  }
  else if (direction < 0 || direction > 180)
  {
    collision =  CollisionCheckType::Left;
  }
  else if (direction > 0 && direction <= 180)
  {
    collision =  CollisionCheckType::Right;
  }
  else
  {
    collision = CollisionCheckType::Front;
  }
  lastDirectionConsign = direction;
  controlLoop(collision, duration);
}

// Luigi wins!
void TrajectoryControl::doNothing()
{
  #ifdef DEBUG
  Serial.println("Luigi wins !!!");
  #endif
  
  stopMotion();
  while (true) {
    checkTimerAndEndProgramIfNeeded();
    delay(MIN_PULSE_DELAY);
  }
}

// no fine control of speed profile (yet)
void TrajectoryControl::controlLoop(CollisionCheckType checkForCollisions, int duration)
{
  #ifdef DEBUG
  Serial.print("Control loop for :");
  Serial.println(duration);
  Serial.println("lastDirectionConsign:");
  Serial.println(lastDirectionConsign);
  #endif

  //int sonar;
  //setupUltrasoundResources(checkForCollisions, sonar);
  int loopBegin = millis();

  bool slowed = false;

  int timeWaiting = 0;

  while (millis() < loopBegin + duration + timeWaiting) {
    unsigned long pulseTime = millis();

    if (pulseTime > loopBegin + duration * 0.8 && !slowed)
    {
      // reduce speed by half if translating
      if (lastDirectionConsign != 0xFFFF)
      {
        linearSpeedConsign(REDUCED_LINEAR_SPEED_CONSIGN, lastDirectionConsign);
        currentLinearSpeed = REDUCED_LINEAR_SPEED_CONSIGN;
        slowed = true;
      }
    }

    int dist = pulse(checkForCollisions);
    #ifdef DEBUG
    Serial.print("ControlLoop::Distance: ");
    Serial.println(dist);
    #endif
    checkTimerAndEndProgramIfNeeded();

    if ((checkForCollisions != CollisionCheckType::None) && (!isDistanceSafe(dist))) {
      stopMotion();
      int startWait = millis();
      #ifdef DEBUG
      Serial.println("OSKOUR OSKOUR OSKOUR !!!");
      #endif
      waitForFreePath(checkForCollisions);
      timeWaiting = millis() - startWait + 500;
      resumeMotion();
    }
  }

  stopMotion();
  
  #ifdef DEBUG
  Serial.println("==== I'M OUTTA UR LOOP ===========================");
  #endif
}

//kill motors
void TrajectoryControl::stopMotion()
{
  #ifdef DEBUG
  Serial.println("### Scrooge's hill ###");
  #endif
  sendSpeedConsigns(0, 0, 0);
}


//resume old direction
void TrajectoryControl::resumeMotion()
{
  linearSpeedConsign(currentLinearSpeed, lastDirectionConsign);
}

// WARNING - Blocking call
// return distance (cm)
int TrajectoryControl::pulse(CollisionCheckType coll)
{
  //TODO HACK HACK HACK
  return 1200;
  // /HACK /HACK /HACK

  
  int pulsetime;
  switch (coll) {
    case CollisionCheckType::Front:
      pinMode(A1, OUTPUT);
      pulsetime = millis();
      digitalWrite(A1, HIGH);
      delayMicroseconds(10);
      digitalWrite(A1, LOW);
      pinMode(A1, INPUT);
      delayMicroseconds(10);    
      return (pulseIn(A1, HIGH) / 58);
      
    case CollisionCheckType::Right:
      pinMode(A5, OUTPUT);
      pulsetime = millis();
      digitalWrite(A5, HIGH);
      delayMicroseconds(10);
      digitalWrite(A5, LOW);
      pinMode(A5, INPUT);
      delayMicroseconds(10);    
      return (pulseIn(A5, HIGH) / 58);
    case CollisionCheckType::Left:
      pinMode(A0, OUTPUT);
      pulsetime = millis();
      digitalWrite(A0, HIGH);
      delayMicroseconds(10);
      digitalWrite(A0, LOW);
      pinMode(A0, INPUT);
      delayMicroseconds(10);    
      return (pulseIn(A0, HIGH) / 58);
    default:
      //pin = 0xFFFF;
      return -1;
  }
  
//  #ifdef DEBUG
//  Serial.print(">>> Pulsing on pin ");
//  Serial.println(pin);
//  #endif
//  
//  if (pin = 0xFFFF)
//    return -1;
//    
//  pinMode(pin, OUTPUT);
//  int pulsetime = millis();
//  digitalWrite(pin, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(pin, LOW);
//  pinMode(pin, INPUT);
//  delayMicroseconds(10);
//
//  return (pulseIn(pin, HIGH) / 58);
}

//uses the sonar computed when setting up the motion direction
void TrajectoryControl::waitForFreePath(CollisionCheckType checkForCollisions)
{
  #ifdef DEBUG
  Serial.println("Waiting for path to clear.");
  #endif
  while (true) {
    checkTimerAndEndProgramIfNeeded();

    int dist = pulse(checkForCollisions);
    #ifdef DEBUG
    Serial.print("BLOCKED - dist = ");
    Serial.println(dist);
    #endif
    if (isDistanceSafe(dist)) {
      #ifdef DEBUG
      Serial.println("Iz ok now :)");
      #endif
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
  //return true;
  #ifdef DEBUG
  if(distance == 0)
    Serial.println("WARNING !!! US Module probably off !!!");
  #endif
  return (distance <= 0) || (distance >= DANGER_DISTANCE);
}

//
//inline int TrajectoryControl::degAtan2(int y, int x)
//{
//  return (int)(360 + atan2((double)y, (double)x) * 45 / atan(1.0));
//}
//
inline int TrajectoryControl::degCos(int r, int theta)
{
  return (int)(r * cos((double)theta * atan(1.0) / 45));
}
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


// In the general case (Both translation and rotation required), the contribution for each motor
// is the sum of the linear and angular contributions.
// NOTE: there should be a limit on SUM(linSpeed,angSpeed) to ensure no saturation occurs in the consign
void TrajectoryControl::generalSpeedConsign(int linSpeed, int direction, int angSpeed, Rotation rot)
{
  Rotation actualRot = rot;
  //TODO: check base side
  if (_side == TeamSide::Left)
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
  int rotSpeed = angSpeed / ROBOT_RADIUS * (actualRot == Rotation::Clockwise ? -1 : 1);
  int mot1 = linSpeed * cos(WHEEL_1_DIRECTION - direction) + rotSpeed;
  int mot2 = linSpeed * cos(WHEEL_2_DIRECTION - direction) + rotSpeed;
  int mot3 = linSpeed * cos(WHEEL_3_DIRECTION - direction) + rotSpeed;
  sendSpeedConsigns(mot1, mot2, mot3);
}


// read encoders
long TrajectoryControl::readEncoder(int enc)
{
  //return sendMotorCommand(CMD_READ_ENC, enc, 0);
  return Encoder.readChannel((char) enc);
}

void TrajectoryControl::setupUltrasoundResources(CollisionCheckType checkForCollisions, int& sonar)
{
  switch (checkForCollisions) {
    case CollisionCheckType::Front:
      sonar = PIN_ECHO_FRONT;
      break;
    case CollisionCheckType::Right:
      sonar = PIN_ECHO_120;
      break;
    case CollisionCheckType::Left:
      sonar = PIN_ECHO_240;
      break;
    default:
      sonar = 0xFFFF;
  }
  
  #ifdef DEBUG
  Serial.print("Selected sonar for this move: ");
  switch (checkForCollisions) {
    case CollisionCheckType::Front:
      Serial.print(sonar);
      Serial.print(A1);
      Serial.println(" FRONT");
      break;
    case CollisionCheckType::Right:
      Serial.print(sonar);
      Serial.print(A5);
      Serial.println(" Right");
      break;
    case CollisionCheckType::Left:
      Serial.print(sonar);
      Serial.print(A0);
      Serial.println(" Left");
      break;
    default:
      Serial.print(sonar);
      Serial.println(" NONE !");
  }
  #endif
}
