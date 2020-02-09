#include "motor.h"

void setup()
{
  Serial.begin(9600);
  initialize_motor();
  flag_pole_init();
}

void debug_turning()
{
  set_motor_target(TURNING_MODE, -180);
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }
  delay(1000);
  
  set_motor_target(TURNING_MODE, -180);
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }
  delay(1000);
}

void debug_forward()
{
  set_motor_target(FORWARD_MODE, 400);
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }
  delay(1000);
  
  set_motor_target(FORWARD_MODE, -400);
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }
  delay(1000);
}

void loop()
{
  debug_turning();
  // debug_forward();
}
