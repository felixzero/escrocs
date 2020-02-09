#include "flag_pole.h"

void setup()
{
  flag_pole_init();
}

void loop()
{
  flag_pole_raise();
  delay(5000);
  flag_pole_lower();
  delay(5000);
}
