#include <SPI.h>

#include "radio.h"

#define SS_PIN 10
#define INTERRUPT_PIN 2

static radio_payload_t radio_payload;


static void write_register(uint8_t reg, uint8_t value)
{
  digitalWrite(SS_PIN, LOW);
  SPI.transfer(reg | 0x80);
  SPI.transfer(value);
  digitalWrite(SS_PIN, HIGH);
}


static uint8_t read_register(uint8_t reg)
{
  uint8_t output;
  
  digitalWrite(SS_PIN, LOW);
  SPI.transfer(reg & 0x7F);
  output = SPI.transfer(0x00);
  digitalWrite(SS_PIN, HIGH);

  return output;
}


static void radio_interrupt_handler(void)
{
  char *payload_ptr = (char*)&radio_payload;
  for (int i = 0; i < PAYLOAD_LENGTH; i++) {
    payload_ptr[i] = read_register(0x00);
  }
}


void initialize_radio_receiver(void)
{
  radio_payload.flags = 0x00;
  
  pinMode(SS_PIN, OUTPUT);
  SPI.begin();

  // Configure radio through SPI
  for (int i = 0; i < ARRAY_SIZE(COMMON_RF_PARAMETERS); i += 2)
    write_register(COMMON_RF_PARAMETERS[i], COMMON_RF_PARAMETERS[i + 1]);
  
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), radio_interrupt_handler, RISING);

  // Put radio in listen mode
  write_register(0x01, 0x10);
}
