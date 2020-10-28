#pragma once

#include <Arduino.h>

// Everything in mm

// Robot dimensions
#define ROBOT_HALF_SIDE            120
#define ROBOT_SIDE                 (ROBOT_HALF_SIDE * 2)
#define WHEEL_SAFETY_DISTANCE       50
#define CUP_ENTRANCE_OFFSET_EDGE    70
#define CUP_ENTRANCE_DEPTH         100
#define CUP_ENTRANCE_OFFSET_CENTER (ROBOT_SIDE / 2 - CUP_ENTRANCE_OFFSET_EDGE)

// Field dimensions
#define FIELD_X              3000
#define FIELD_Y              2000
#define GREEN_CHANNEL_Y       515
#define RED_CHANNEL_Y        1085

#define DANGER_DISTANCE  300


#define UNCHANGED -1
#define DONTCARE  -2

typedef bool (*timerCallback_t)(void);

class TrajectoryControl
{
public:
  enum class CollisionCheckType {
    Front,
    Rear,
    None
  };

  enum class TeamSide {
    Left,
    Right
  };

  void begin(int initialX, int initialY, int initialTheta, TeamSide side, timerCallback_t timerCallback);

  // Absolute moves
  void moveTo(int destinationX, int destinationY, int destinationTheta);
  void moveTo(int destinationX, int destinationY) { moveTo(destinationX, destinationY, DONTCARE); }
  void rotateToAngle(int destinationTheta) { moveTo(UNCHANGED, UNCHANGED, destinationTheta); }
  void doNothing();

  // Relative moves
  void rotate(int angle);
  void translate(int distance);
  void translateWithoutCheck(int distance);

  // Position getters
  int getCurrentX() const { return _currentX; }
  int getCurrentY() const { return _currentY; }
  int getCurrentTheta() const { return _currentTheta; }

private:
  void controlLoop(CollisionCheckType checkForCollisions);
  void waitForFreePath();
  void setupUltrasoundResources(CollisionCheckType checkForCollisions, int& left, int& right);
  void checkTimerAndEndProgramIfNeeded();

  static inline int degAtan2(int y, int x);
  static inline int degCos(int r, int theta);
  static inline int degSin(int r, int theta);
  static inline int intDistance(int x, int y);
  static inline int moduloAngle(int theta);
  static inline bool isDistanceSafe(int distance);

  int _currentX = -1, _currentY = -1, _currentTheta = -1;
  TeamSide _side = TeamSide::Left;
  timerCallback_t _timerCallback;
};

extern TrajectoryControl Trajectory;
