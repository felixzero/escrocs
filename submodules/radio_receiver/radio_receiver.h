#pragma once

#include "radio_common.h"

class RadioReceiver
{
public:
  void begin();
  const radio_payload_t& payload() { return radioPayload; }

private:
  void writeRegister(uint8_t reg, uint8_t value);
  uint8_t readRegister(uint8_t reg);
  static void interruptHandler();

  radio_payload_t radioPayload;
};

extern RadioReceiver Radio;



