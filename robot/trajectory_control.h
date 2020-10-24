#pragma once

// Everything in mm

// Robot dimensions
#define ROBOT_SIDE                 240
#define ROBOT_HALF_SIDE            120
#define WHEEL_SAFETY_DISTANCE       50
#define CUP_ENTRANCE_OFFSET_EDGE    70
#define CUP_ENTRANCE_DEPTH         100
#define CUP_ENTRANCE_OFFSET_CENTER (ROBOT_SIDE / 2 - CUP_ENTRANCE_OFFSET_EDGE)

// Field dimensions
#define FIELD_X              3000
#define FIELD_Y              2000
#define GREEN_CHANNEL_Y 515
#define RED_CHANNEL_Y 1085

#define DANGER_DISTANCE  300


#define UNCHANGED -1
#define DONTCARE  -2

class TrajectoryControl
{
public:
  void begin(int initialX, int initialY, int initialTheta);

  // Absolute moves
  void moveTo(int destinationX, int destinationY, int destinationTheta);
  void moveTo(int destinationX, int destinationY) { moveTo(destinationX, destinationY, DONTCARE); }
  void rotateToAngle(int destinationTheta) { moveTo(UNCHANGED, UNCHANGED, destinationTheta); }

  // Relative moves
  void rotate(int angle);
  void moveForward(int distance);

  // Position getters
  int getCurrentX() const { return _currentX; }
  int getCurrentY() const { return _currentY; }
  int getCurrentTheta() const { return _currentTheta; }

private:
  void controlLoop();
  void waitForFreePath();

  static inline int DEG_ATAN2(int y, int x);
  static inline int DEG_COS(int r, int theta);
  static inline int DEG_SIN(int r, int theta);
  static inline int INT_DISTANCE(int x, int y);
  static inline bool IS_DISTANCE_SAFE(int distance);

  int _currentX = -1, _currentY = -1, _currentTheta = -1;
};

extern TrajectoryControl Trajectory;
