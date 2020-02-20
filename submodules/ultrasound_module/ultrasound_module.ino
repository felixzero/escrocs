#include "ultrasound.h"

void setup()
{
  Serial.begin(9600);
  UltraSounds.begin();
}

void loop()
{
  UltraSounds.pulse(0xFF);
  delayMicroseconds(60000);

  Serial.print("1> ");
  Serial.println(UltraSounds.read(ULTRASOUND_FL));
  Serial.print("2> ");
  Serial.println(UltraSounds.read(ULTRASOUND_FR));
  Serial.println();

  delay(1000);
}
