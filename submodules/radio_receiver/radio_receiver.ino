#include <SPI.h>

#include "radio_receiver.h"

void setup()
{
  Serial.begin(9600);
  Serial.println("Radio demo for duino");
  delay(10);

  Radio.begin();
}


void loop()
{
  Serial.print("Flags: ");
  Serial.println(Radio.payload().flags);
  delay(500);
}
