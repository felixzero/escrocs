#pragma once

#define NUMBER_OF_SERVO_CHANNELS 16

// API for the TLC5940 custom board
// Used pins to be set in pinout.h
class ServoBoard
{
public:
  // Initialize the submodule
  void begin();
  
  // Set value for a specific servo channel
  void write(byte channel, int value);

private:
  void resendSerialData();
  
  word channelValues[NUMBER_OF_SERVO_CHANNELS];
};

extern ServoBoard Servos;
