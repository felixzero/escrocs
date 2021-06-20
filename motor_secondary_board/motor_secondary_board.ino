#include <Wire.h>

#include "motor.h"
#include "encoder.h"

#define I2C_ADDR 42

#define CMD_READ_ENC 1
#define CMD_WRITE_SPEED 2

void setup() {
    Wire.begin(I2C_ADDR);
    Wire.onReceive(i2cCallback);
  
    Motor.begin();
    Encoder.begin();
    Serial.begin(9600);
}

void loop() {
  // Nothing to loop
  delay(100);
}

void i2cCallback(int howMany) {
    while (howMany >= 2) {
      manageI2cCommand(Wire.read(), Wire.read());
      howMany -= 2;
    }
}

void manageI2cCommand(uint8_t command, uint8_t argument) {
  uint8_t queryChannel = command & 0x3;
  uint8_t queryCommand = command >> 2;

  switch (queryCommand) {
    case CMD_READ_ENC:
      writeLong(Encoder.readChannel(queryChannel));
      break;

    case CMD_WRITE_SPEED:
      Motor.writeSpeed(queryChannel, argument);
      writeLong(Encoder.readChannel(queryChannel));
      break;

    default:
      writeLong(0xFFFFFFFF);
  }
}

void writeLong(long value) {
  for (int i = 0; i < 4; ++i) {
    Wire.write((char)value);
    value >>= 8;
  }
}
