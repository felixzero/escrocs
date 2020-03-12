#include <SPI.h>

#include "servos.h"

void setup()
{
  Serial.begin(9600);
  
  Servos.begin();
  delay(500);
}

void loop()
{
  for (int i = 1000; i < 2000; i += 10) {
      Servos.writeMicroseconds(1, i);
      delay(100);
  }
}
