#include <Servo.h>

#include "flag_pole.h"
#include "pinout.h"

#define RAISED_POSITION 70
#define LOWERED_POSITION 28

static Servo flag_servo;

void flag_pole_init(void)
{
  flag_servo.attach(PIN_FLAG_SERVO);
  flag_pole_lower();
}

void flag_pole_raise(void)
{
  flag_servo.write(130);
}

void flag_pole_lower(void)
{
  flag_servo.write(LOWERED_POSITION);
}
