#include "omni_trajectory_control.h"

#define PIN_STARTER_OUT 0
#define PIN_STARTER_IN  1
#define PIN_COLOR      A4

static unsigned long gameStartTime = 0;
static TrajectoryControl::TeamSide teamSide = TrajectoryControl::TeamSide::Left;

void setup() {
    //Serial.begin(9600);    
    Trajectory.begin(ROBOT_HALF_SIDE, (GREEN_CHANNEL_Y + RED_CHANNEL_Y) / 2, 0, teamSide, timerCallback);

    pinMode(PIN_STARTER_OUT, OUTPUT);
    digitalWrite(PIN_STARTER_OUT, LOW);
    pinMode(PIN_STARTER_IN, INPUT_PULLUP);
    pinMode(PIN_COLOR, INPUT_PULLUP);
    
    waitForStartSignal();
}

void loop() {
  //Serial.println("Welcome to OmniBot !!!");

  // BEGIN CODE HOMOLOGUE 
  /*Trajectory.translate(120, 1400);
  Trajectory.doNothing();*/
  // END CODE HOMOLOGUE

  // USEFUL TIMERS ===============================================================
  //U-Turn
  // Trajectory.rotate(255, TrajectoryControl::Rotation::Counterclockwise, 900);
  //360°
  // Trajectory.rotate(255, TrajectoryControl::Rotation::Counterclockwise, 1750);
  //==============================================================================

  // BEGIN CODE MATCH "WINDBUOY KILLER" v1
  TrajectoryControl::TeamSide color = (digitalRead(PIN_COLOR) == LOW) ? TrajectoryControl::TeamSide::Left : TrajectoryControl::TeamSide::Right;
  
  if(color == TrajectoryControl::TeamSide::Left) // Blue
  {
    Trajectory.translate(120, 2900);
    delay(500);
    // Trajectory.rotate(255, TrajectoryControl::Rotation::Counterclockwise, 1750);
    //(+120) FL, FR, R
    Trajectory.sendSpeedConsigns(255, 255, -255);
    Trajectory.controlLoop(TrajectoryControl::CollisionCheckType::Left, 1000);
    delay(500);
    Trajectory.rotate(255, TrajectoryControl::Rotation::Counterclockwise, 300);
    delay(500);
    Trajectory.translate(120, 1500);
    delay(500);
    Trajectory.sendSpeedConsigns(255, 255, -255);
    Trajectory.controlLoop(TrajectoryControl::CollisionCheckType::Left, 500);
    delay(500);
    Trajectory.doNothing();
  }
  else //Yellow
  {
    Trajectory.translate(120, 2900);
    delay(500);
    //(+120) FL, FR, R
    Trajectory.sendSpeedConsigns(-255, -255, 255);
    Trajectory.controlLoop(TrajectoryControl::CollisionCheckType::None, 1000);
    delay(500);
    Trajectory.rotate(255, TrajectoryControl::Rotation::Clockwise, 300);
    delay(500);
    Trajectory.translate(120, 1500);
    delay(500);
    Trajectory.sendSpeedConsigns(-255, -255, 255);
    Trajectory.controlLoop(TrajectoryControl::CollisionCheckType::Left, 1000);
    delay(500);
    Trajectory.doNothing();
  }
  // END CODE MATCH
  

  // BEGIN CODE MATCH "WINDBUOY KILLER" v0.1
  /*Trajectory.translate(120, 2900);
  delay(500);
  Trajectory.translate(0, 2100);
  delay(500);
  Trajectory.translate(-120, 800);
  delay(500);
  Trajectory.translate(120, 1200);
  delay(500);
  Trajectory.translate(0, 2000);
  delay(500);
  Trajectory.translate(-120, 800);
  //END
  Trajectory.doNothing();*/
  // END CODE MATCH

  /*
  Trajectory.translate(120, 3000);
  //Stabilize
  delay(200);
  //Trajectory.rotate(255, TrajectoryControl::Rotation::Counterclockwise, 800);
  //Trajectory.translate(-90, 4000);
  //Trajectory.translate( 90, 4000);
  Trajectory.sendSpeedConsigns(-255, -255, 0);
  Trajectory.controlLoop(TrajectoryControl::CollisionCheckType::Left, 10000);*/
  
  //Trajectory.translate(0, 1800);
  Trajectory.doNothing();
}

void waitForStartSignal()
{
  delay(500);
  while (digitalRead(PIN_STARTER_IN) == LOW);

  gameStartTime = millis();
  teamSide = (digitalRead(PIN_COLOR) == LOW) ? TrajectoryControl::TeamSide::Left : TrajectoryControl::TeamSide::Right;
}


bool timerCallback()
{
  unsigned long elapsedTime = millis() - gameStartTime;

  // One second of margin of error
  return (elapsedTime < 99000);
}
