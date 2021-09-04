#include <Arduino.h>
#include "wind_info.h"

WindInfo WindInfos;

// Initialize the pin connected to raspi
void WindInfo::begin(char pinValue, char pinReliability)
{
  _pinValue = pinValue;
  _pinReliability = pinReliability;
  pinMode(_pinValue,       INPUT);
  pinMode(_pinReliability, INPUT);
}

WindInfo::Status WindInfo::detectWind(bool useReliability)
{
  int state    = digitalRead(_pinValue);
  int reliable = digitalRead(_pinReliability);
  if(useReliability && reliable == LOW)
    return Undetected;
  if (state == HIGH)
      return South;
  else
      return North;
}
