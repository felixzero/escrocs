#pragma once

#include <Arduino.h>

class WindInfo
{
private:
  char _pinValue;
  char _pinReliability;
  
public:
  enum Status { Undetected = 0, North = 1, South = 2 };

  // Initialize module.
  void begin(char pinValue, char pinReliability);

  // Return the wind state based on raspi provided data
  Status detectWind(bool useReliability);
};

extern WindInfo WindInfos;
