#include <Arduino.h>

#include "motor.h"
#include "encoder.h"

void setup() {
  Serial.begin(9600);

  init_encoders();
  init_motors();

  write_motor_speed(-0.1, 0.2, 0.0);
}

void loop() {
  Serial.println("Loop");
  int16_t channel1, channel2, channel3;
  read_encoders(&channel1, &channel2, &channel3);

  Serial.print(channel1);
  Serial.print(" ");

  Serial.print(channel2);
  Serial.print(" ");

  Serial.print(channel3);
  Serial.println(" ");

  delay(500);
}
