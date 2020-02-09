#include "ultrasound.h"

void setup()
{
  Serial.begin(9600);
  ultrasound_init();
}

void loop()
{
  ultrasound_pulse(0xFF);
  delayMicroseconds(60000);

  Serial.print("1> ");
  Serial.println(ultrasound_read(ULTRASOUND_READ_FL));
  Serial.print("2> ");
  Serial.println(ultrasound_read(ULTRASOUND_READ_FR));
  Serial.println();

  delay(1000);
}
