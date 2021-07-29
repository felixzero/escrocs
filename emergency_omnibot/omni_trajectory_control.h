#pragma once

#include <Arduino.h>
//#include <Wire.h>

// Everything in mm

// Robot dimensions
#define ROBOT_HALF_SIDE             120 //TBC
#define ROBOT_SIDE                  (ROBOT_HALF_SIDE * 2) //TBC
// -- Length from the wheels to center of the triangle
#define ROBOT_RADIUS                130 //TBC

#define WHEEL_SAFETY_DISTANCE       50

//TODO: normally defined in US module
#define MIN_PULSE_DELAY 80 // ms
#define PIN_ECHO_FRONT    A1
#define PIN_ECHO_120      A5
#define PIN_ECHO_240      A0


// Motors angles - Two wheels on front; Front is 0°, axles counted counterclockwise
#define AXLE_1_ANGLE         60  //°
#define AXLE_2_ANGLE        180  //°
#define AXLE_3_ANGLE        -60  //°
// Wheels are perpendicular to motor axis (TODO: check positive direction)
#define WHEEL_1_DIRECTION    AXLE_1_ANGLE+90
#define WHEEL_2_DIRECTION    AXLE_2_ANGLE+90
#define WHEEL_3_DIRECTION    AXLE_3_ANGLE+90

// Field dimensions
#define FIELD_X              3000
#define FIELD_Y              2000
#define GREEN_CHANNEL_Y       515
#define RED_CHANNEL_Y        1085

#define DANGER_DISTANCE  50 // cm

#define UNCHANGED -1
#define DONTCARE  -2

////== I2C communication with motor board =========
//#define MOTORS_ADDR 42
//
//#define CMD_READ_ENC 1
//#define CMD_WRITE_SPEED 2
//#define ANSWERSIZE 32 //long size
////===============================================

typedef bool (*timerCallback_t)(void);

class TrajectoryControl
{
public:
  //The sonar mast looks towards three possible directions, 120° from each others
  enum class CollisionCheckType {
    Front,
    Right,
    Left,
    None
  };

  enum class TeamSide {
    Left,
    Right
  };

  enum class Rotation {
    Clockwise = -1,
    Counterclockwise = 1
  };

  void begin(int initialX, int initialY, int initialTheta, TeamSide side, timerCallback_t timerCallback);

  // Absolute moves
  //void moveTo(int destinationX, int destinationY, int destinationTheta);
  //void moveTo(int destinationX, int destinationY) { moveTo(destinationX, destinationY, DONTCARE); }
  //void rotateToAngle(int destinationTheta) { moveTo(UNCHANGED, UNCHANGED, destinationTheta); }
  void doNothing();

  // Relative moves
  void rotate(int angularSpeed, Rotation rot, int duration);
  void translate(int direction, int duration);
  //void translateWithoutCheck(int distance);

  // Position getters
  //int getCurrentX() const { return _currentX; }
  //int getCurrentY() const { return _currentY; }
  //int getCurrentTheta() const { return _currentTheta; }
  
  //publicized
  void controlLoop(CollisionCheckType checkForCollisions, int duration);
  void sendSpeedConsigns(int mot1, int mot2, int mot3);

private:
  void setupUltrasoundResources(CollisionCheckType checkForCollisions, int& sonar);
  void checkTimerAndEndProgramIfNeeded();
  void waitForFreePath(CollisionCheckType checkForCollisions);

  void linearSpeedConsign(int speed, int direction);
  void generalSpeedConsign(int linSpeed, int direction, int angSpeed, Rotation rot);
  void stopMotion();
  void resumeMotion();
  
  //Send command over I2C
  //void sendMotorCommand(int cmd, long args);
  //void sendSpeedConsigns(int mot1, int mot2, int mot3);
  long readEncoder(int enc);

  //Sonar detection
  int pulse(CollisionCheckType coll);

//  static inline int degAtan2(int y, int x);
  static inline int degCos(int r, int theta);
//  static inline int degSin(int r, int theta);
//  static inline int intDistance(int x, int y);
//  static inline int moduloAngle(int theta);
  static inline bool isDistanceSafe(int distance);

  int _currentX = -1, _currentY = -1, _currentTheta = -1;
  TeamSide _side = TeamSide::Left;
  timerCallback_t _timerCallback;

  int lastDirectionConsign;
  int currentLinearSpeed;
};

extern TrajectoryControl Trajectory;
