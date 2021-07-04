#include "tri_ultrasound.h"

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
  Serial.println(UltraSounds.read(ULTRASOUND_FRONT));
  Serial.print("2> ");
  Serial.println(UltraSounds.read(ULTRASOUND_RIGHT));
  Serial.print("3> ");
  Serial.println(UltraSounds.read(ULTRASOUND_LEFT));
  Serial.println();

  delay(1000);
}
