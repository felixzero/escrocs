#include <SPI.h>

#include "servos.h"

void setup()
{
  Servos.begin();
  delay(500);
}

void loop()
{
  Servos.write(1, 255);
  delay(3000);
  Servos.write(1, 0);
  delay(3000);
}



