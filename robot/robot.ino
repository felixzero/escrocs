 #include "motor.h"
#include "flag_pole.h"

void move_motor(int mode, long pos)
{
  set_motor_target(mode, pos);
  while (!motor_is_idle()) {
    poll_motor_regulation();
  }
  delay(500);
}

void setup()
{
  Serial.begin(9600);
  initialize_motor();
  flag_pole_init();

  move_motor(FORWARD_MODE, 400);
  move_motor(TURNING_MODE, -180);
  move_motor(FORWARD_MODE, 400);
  move_motor(FORWARD_MODE, -400);
  move_motor(TURNING_MODE, 90);
  move_motor(FORWARD_MODE, 550);
  move_motor(TURNING_MODE, -90);
  move_motor(FORWARD_MODE, 400);
  flag_pole_raise();
}

void loop()
{
  // Nothing to loop
}
