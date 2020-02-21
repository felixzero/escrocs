#include <SPI.h>

#include "servos.h"

void setup()
{
  Serial.begin(9600);
  
  Servos.begin();
  delay(500);

  Servos.write(1, 0);
  Servos.write(4, -128);
}

void loop()
{
  for (int i = -128; i < 128; i += 4) {
      Servos.write(4, i);
      delay(100);
  }
  
  //Servos.write(2, 0);
  //delay(500);
  
  /*Servos.write(1, 255);
  Servos.write(4, 0);
  delay(3000);
  Servos.write(4, 255);
  Servos.write(1, 0);
  delay(3000);*/
}
