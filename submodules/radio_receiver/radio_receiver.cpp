#include <SPI.h>
#include <Arduino.h>

#include "radio_receiver.h"
#include "pinout.h"

RadioReceiver Radio;

void RadioReceiver::writeRegister(uint8_t reg, uint8_t value)
{
  digitalWrite(PIN_SPI_SS, LOW);
  SPI.transfer(reg | 0x80);
  SPI.transfer(value);
  digitalWrite(PIN_SPI_SS, HIGH);
}

uint8_t RadioReceiver::readRegister(uint8_t reg)
{
  uint8_t output;
  
  digitalWrite(PIN_SPI_SS, LOW);
  SPI.transfer(reg & 0x7F);
  output = SPI.transfer(0x00);
  digitalWrite(PIN_SPI_SS, HIGH);

  return output;
}

void RadioReceiver::interruptHandler()
{
  char *payloadPtr = (char*)&Radio.radioPayload;
  for (int i = 0; i < PAYLOAD_LENGTH; i++) {
    payloadPtr[i] = Radio.readRegister(0x00);
  }
}

void RadioReceiver::begin()
{
  radioPayload.flags = 0x00;
  
  pinMode(PIN_SPI_SS, OUTPUT);
  SPI.begin();

  // Configure radio through SPI
  for (int i = 0; i < ARRAY_SIZE(COMMON_RF_PARAMETERS); i += 2)
    writeRegister(COMMON_RF_PARAMETERS[i], COMMON_RF_PARAMETERS[i + 1]);
  
  attachInterrupt(digitalPinToInterrupt(PIN_RADIO_INT), &RadioReceiver::interruptHandler, RISING);

  // Put radio in listen mode
  writeRegister(0x01, 0x10);
}


