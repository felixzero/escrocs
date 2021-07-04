#pragma once

#include <Arduino.h>
#include <Wire.h>

// Everything in mm

// Robot dimensions
#define ROBOT_HALF_SIDE             120 //TBC
#define ROBOT_SIDE                  (ROBOT_HALF_SIDE * 2) //TBC
// -- Lenght from the wheels to center of the triangle
#define ROBOT_RADIUS                100 //TBC

#define WHEEL_SAFETY_DISTANCE       50


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

#define DANGER_DISTANCE  300

#define UNCHANGED -1
#define DONTCARE  -2

//== I2C communication with motor board =========
#define MOTORS_ADDR 42

#define CMD_READ_ENC 1
#define CMD_WRITE_SPEED 2
#define ANSWERSIZE 32 //long size
//===============================================

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

private:
  void controlLoop(CollisionCheckType checkForCollisions, int duration);
  void setupUltrasoundResources(CollisionCheckType checkForCollisions, int& sonar);
  void checkTimerAndEndProgramIfNeeded();
  void waitForFreePath(int sonar);

  void linearSpeedConsign(int speed, int direction);
  void generalSpeedConsign(int linSpeed, int direction, int angSpeed, Rotation rot);
  void stopMotion();
  void resumeMotion();
  
  //Send command over I2C
  void sendMotorCommand(int cmd, long args);
  void sendSpeedConsigns(int mot1, int mot2, int mot3);
  long readEncoder(int enc);

//  static inline int degAtan2(int y, int x);
//  static inline int degCos(int r, int theta);
//  static inline int degSin(int r, int theta);
//  static inline int intDistance(int x, int y);
//  static inline int moduloAngle(int theta);
  static inline bool isDistanceSafe(int distance);

  int _currentX = -1, _currentY = -1, _currentTheta = -1;
  TeamSide _side = TeamSide::Left;
  timerCallback_t _timerCallback;

  static int lastDirectionConsign;
};

extern TrajectoryControl Trajectory;
