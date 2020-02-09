#include <Arduino.h>
#include <Wire.h>

#include "motor.h"

#define MOTOR_I2C_ADDR 0x58

#define MOTOR_I2C_SPEED1  (byte)0x00
#define MOTOR_I2C_SPEED2  0x01
#define MOTOR_I2C_ENC1A   0x02
#define MOTOR_I2C_ENC2A   0x06
#define MOTOR_I2C_ACCEL   0x0D
#define MOTOR_I2C_MODE    0x0E
#define MOTOR_I2C_COMMAND 0x0F

#define MOTOR_COMMAND_RST_ENCODER 0x20

#define SIGN(x) ((x > 0) ? 1 : -1)

#define FORWARD_FEEDBACK 400
#define TURNING_FEEDBACK 400

static int motion_mode = IDLE_MODE;
static long target_position = 0;
static long initial_enc1, initial_enc2;

// Sent a R/W register over the I2C bus
inline void _send_motor_command(char reg, char value)
{
  Wire.beginTransmission(MOTOR_I2C_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

// Read a multibyte register from the I2C bus
long _read_motor_register(char reg, char reg_size)
{
  Wire.beginTransmission(MOTOR_I2C_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(MOTOR_I2C_ADDR, reg_size);
  while (Wire.available() < reg_size)
    ;
  long reading = 0;
  for (int i = 0; i < reg_size; ++i) {
    reading <<= 8;
    reading += Wire.read();
  }
  
  return reading;
}

void initialize_motor()
{
  // I2C motor card initialization
  Wire.begin();
  
  _send_motor_command(MOTOR_I2C_MODE, 1);
  _send_motor_command(MOTOR_I2C_COMMAND, MOTOR_COMMAND_RST_ENCODER);

  // Needed for the MD25 card to "stabilize"
  delay(1000);
}

void set_raw_motor_target(int mode, long raw_pos)
{
  motion_mode = mode;
  target_position = raw_pos;

  // Save encoder positions
  initial_enc1 = _read_motor_register(MOTOR_I2C_ENC1A, 4);
  initial_enc2 = _read_motor_register(MOTOR_I2C_ENC2A, 4);
}

void set_motor_target(int mode, long pos)
{
  if (mode == FORWARD_MODE) {
   set_raw_motor_target(mode, 4 * (pos * 1000) / MOTOR_FWD_MOTION_PER_TICK);
  } else if (mode == TURNING_MODE) {
    set_raw_motor_target(mode, 2 * (pos * 1000) / MOTOR_ROTATION_PER_TICK);
  }
}

void poll_motor_regulation()
{
  static long last_target_speed = 0;
  static long last_error = 0;
  
  // Read encoder positions
  long enc1 = _read_motor_register(MOTOR_I2C_ENC1A, 4);
  long enc2 = _read_motor_register(MOTOR_I2C_ENC2A, 4);
  
  long motion, error;
  long feedback = (motion_mode == FORWARD_MODE) ? FORWARD_FEEDBACK : TURNING_FEEDBACK;

  if (motion_mode == IDLE_MODE)
    return;

  // Calculate desired motion and error
  if (motion_mode == FORWARD_MODE) {
    motion = (enc1 - initial_enc1) + (enc2 - initial_enc2);
    error = (enc1 - initial_enc1) - (enc2 - initial_enc2);
  } else if (motion_mode == TURNING_MODE) {
    motion = (enc1 - initial_enc1) - (enc2 - initial_enc2);
    error = (enc1 - initial_enc1) + (enc2 - initial_enc2);
  }

  // Reduce speed if target almost reached
  long distance_to_target = target_position - motion;
  long target_speed = (min(abs(target_position - motion), abs(motion)) * TARGET_SPEED / ACCELERATION_DISTANCE + 1) * SIGN(distance_to_target);

  Serial.println(distance_to_target);

  // Stop motor if target reached and speed is low
  if ((distance_to_target == 0) && (abs(last_target_speed) <= 1) && (last_error <= (1024 / feedback))) {
    stop_motor_motion();
    return;
  }
  
  long correction_speed = error * feedback / 1024;
  last_target_speed = target_speed;
  last_error = error;

  // Send speed target
  if (motion_mode == FORWARD_MODE) {
    _send_motor_command(MOTOR_I2C_SPEED1,  target_speed - correction_speed);
    _send_motor_command(MOTOR_I2C_SPEED2,  target_speed + correction_speed);
  } else if (motion_mode == TURNING_MODE) {
    _send_motor_command(MOTOR_I2C_SPEED1,  target_speed - correction_speed);
    _send_motor_command(MOTOR_I2C_SPEED2, -target_speed - correction_speed);
  }
}

void stop_motor_motion()
{
  motion_mode = IDLE_MODE;
  _send_motor_command(MOTOR_I2C_SPEED1, 0);
  _send_motor_command(MOTOR_I2C_SPEED2, 0);
}

int motor_is_idle()
{
  return (motion_mode == IDLE_MODE);
}
