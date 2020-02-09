#include "radio_receiver.h"

void setup()
{
  Serial.begin(9600);
  Serial.println("Radio demo for duino");
  delay(10);

  initialize_radio_receiver();
}


void loop()
{
  Serial.print("Flags: ");
  Serial.println(radio_payload.flags);
  delay(500);
}
