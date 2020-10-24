#include "trajectory_control.h"
#include "pinout.h"

#define EPSILON        40 // to be able to release a buoy
#define FRONT_MARGIN   20 // to be able to release a buoy
#define REAR_MARGIN    10 // margin for hitting the lighthouse

void basic_strategy()
{
  Trajectory.moveForward(550);
  Trajectory.moveTo(UNCHANGED, GREEN_CHANNEL_Y + CUP_ENTRANCE_OFFSET_CENTER, 180);
  Trajectory.moveTo(ROBOT_SIDE / 2 + 30, UNCHANGED);
  Trajectory.moveForward(-520);
  Trajectory.moveTo(UNCHANGED, RED_CHANNEL_Y - CUP_ENTRANCE_OFFSET_CENTER, 180);
  Trajectory.moveTo(ROBOT_SIDE / 2 + 30, UNCHANGED);
  Trajectory.moveForward(-520);
}

void better_strategy()
{
  // 120, 800, 0
  Trajectory.moveTo(700, (GREEN_CHANNEL_Y + RED_CHANNEL_Y) / 2, DONTCARE); // movement 1
  // 700, 800, 0
  Trajectory.moveTo(UNCHANGED, 455, 180); // movement 2
  // 700, 455, 180
  // open the left and right arms to guide the 2 buoys
  Trajectory.moveTo(ROBOT_HALF_SIDE + EPSILON, UNCHANGED, DONTCARE); // movement 3
  // 160, 455, 180
  // close the right arm to capture the red buoy
  Trajectory.moveForward(-65); // movement 4
  // 225, 455, 180
  // close the left arm (the green buoy should be outside the robot, correctly placed on the channel)
  Trajectory.rotate(-90); // end of movement 4
  // 225, 455, 90
  Trajectory.translateWithoutCheck(-(455 - ROBOT_HALF_SIDE - REAR_MARGIN)); // movement 5
  // 225, 130, 90
  Trajectory.moveForward(-220); // movement 6
  // 225, 350, 90
  Trajectory.moveTo(700, 1140, 180); // movement 7 & 8 combined, check that red buoy is not hit!
  // 700, 1140, 180
  // open the left arm to catch the green buoy
  // open the right arm to guide the second red buoy along the channel
  Trajectory.moveTo(ROBOT_HALF_SIDE + EPSILON, UNCHANGED, DONTCARE); // movement 9
  // 160, 1140, 180
  // close the left arm to capture the green buoy
  // with the next movement, the 2 red buoy will be correctly positioned
  Trajectory.moveForward(-290); // movement 10
  // 450, 1140, 180
  // close the right arm, the 2 red buoy should be correctly positioned on the channel
  Trajectory.moveTo(UNCHANGED, 1800, DONTCARE); // movement 11, approach
  // 450, 1800, -90
  Trajectory.translateWithoutCheck(200); // movement 11, tapping the side, to be tuned, or removed if not necessary (depends on the position error we have at that moment)
  // 450, 1880, -90
  Trajectory.moveForward(-80); // movement 11, moving to the correct y
  // 450, 1800, -90
  Trajectory.moveTo(ROBOT_HALF_SIDE + FRONT_MARGIN, 1850, 180); // movement 12, approach
  // 140, 1800, 180
  // deploy the arm, warning! the arm should be on the other side of the robot if we start on the right of the table! we need to pay attention to this!
  Trajectory.moveForward(-660); // movement 13
  // 800, 1800, 180
  // undeploy the arm
}

void setup()
{
  Serial.begin(9600);
  Trajectory.begin(ROBOT_HALF_SIDE, (GREEN_CHANNEL_Y + RED_CHANNEL_Y) / 2, 0);

  // Wait for start signal
  pinMode(PIN_STARTER, INPUT_PULLUP);
  delay(500);
  while (digitalRead(PIN_STARTER) == LOW);

  better_strategy()
}

void loop()
{
  // Nothing to loop
}
