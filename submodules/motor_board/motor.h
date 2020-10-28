#pragma once

class MotorControl
{
public:
  enum Mode { Idle, Forward, Turning };

  void begin();
  void setRawTarget(Mode mode, long rawPos);
  void setTarget(Mode mode, long pos);
  void pollRegulation();
  void stopMotion();
  void resumeMotion();
  bool isIdle();

private:
  inline void sendCommand(char reg, char value);
  inline long readRegister(char reg, char regSize);

  Mode motionMode = Idle, previousMotionMode;
  long targetPosition = 0;
  long initialEnc1, initialEnc2;
  long lastTargetSpeed = 0;
  long lastError = 0;
  long lastCorrectionSpeed = 0;
};

extern MotorControl Motor;
