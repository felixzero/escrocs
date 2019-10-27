#include "motor.h"

void setup()
{
  initialize_datalog();
  initialize_motor();
}

void loop()
{
/*set_motor_target(TURNING_MODE, 180);
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }
  set_motor_target(TURNING_MODE, 180);
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }

  delay(1000);*/


/*  set_motor_target(FORWARD_MODE, 500);
  while (!motor_is_idle()) {
    poll_motor_regulation();delay(20);
  }

  set_raw_motor_target(TURNING_MODE, get_last_motor_error());
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }
  
  set_motor_target(FORWARD_MODE, -500);
  while (!motor_is_idle()) {
    poll_motor_regulation();delay(20);
  }

  set_raw_motor_target(TURNING_MODE, get_last_motor_error());
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }

  delay(1000);*/
  
  set_motor_target(TURNING_MODE, -90);
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }
  delay(500);

  set_motor_target(FORWARD_MODE, 700);
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }
  delay(500);
  set_motor_target(TURNING_MODE, 180);
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }
  delay(500);
  set_motor_target(FORWARD_MODE, 700);
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }
  delay(500);
  set_motor_target(TURNING_MODE, -90);
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }
  delay(500);
}
