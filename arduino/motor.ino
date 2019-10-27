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

static const long forward_pid[] = { 2, 0, 0 };
static const long turning_pid[] = { 1, 0, 0 };

static int motion_mode = IDLE_MODE;
static long target_position = 0;
static long initial_enc1, initial_enc2, last_error, last_target_speed;

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
  _send_motor_command(MOTOR_I2C_ACCEL, ACCELERATION_RATE);
  _send_motor_command(MOTOR_I2C_COMMAND, MOTOR_COMMAND_RST_ENCODER);

  // Needed for the MD25 card to "stabilize"
  delay(1000);
}

void set_raw_motor_target(int mode, long raw_pos)
{
  motion_mode = mode;
  target_position = raw_pos;
  last_target_speed = 0;

  // Save encoder positions
  initial_enc1 = _read_motor_register(MOTOR_I2C_ENC1A, 4);
  initial_enc2 = _read_motor_register(MOTOR_I2C_ENC2A, 4);
}

void set_motor_target(int mode, long pos)
{
  if (mode == FORWARD_MODE) {
   set_raw_motor_target(mode, 2 * (pos * 1000) / MOTOR_FWD_MOTION_PER_TICK);
  } else if (mode == TURNING_MODE) {
    set_raw_motor_target(mode, (pos * 1000) / MOTOR_ROTATION_PER_TICK);
  }
}

void poll_motor_regulation()
{
  // Read encoder positions
  long enc1 = _read_motor_register(MOTOR_I2C_ENC1A, 4);
  long enc2 = _read_motor_register(MOTOR_I2C_ENC2A, 4);
  long motion, target_speed, correction_speed;
  long *pid_params;

  // Send debug data
  log_datapoint(CHANNEL_MOTOR_ENC1, (float)enc1);
  log_datapoint(CHANNEL_MOTOR_ENC2, (float)enc2);

  if (motion_mode == IDLE_MODE)
    return;

  // Calculate desired motion and error
  if (motion_mode == FORWARD_MODE) {
    motion = ((enc1 - initial_enc1) + (enc2 - initial_enc2)) / 2;
    last_error = ((enc1 - initial_enc1) - (enc2 - initial_enc2)) / 2;
    pid_params = forward_pid;
  } else if (motion_mode == TURNING_MODE) {
    motion = ((enc1 - initial_enc1) - (enc2 - initial_enc2)) / 2;
    last_error = ((enc1 - initial_enc1) + (enc2 - initial_enc2)) / 2;
    pid_params = turning_pid;
  }

  // Reduce speed if target almost reached
  long distance_to_target = target_position - motion;
  if (abs(distance_to_target) > SLOW_DOWN_DISTANCE)
    distance_to_target = SIGN(distance_to_target) * SLOW_DOWN_DISTANCE;
  target_speed = distance_to_target * TARGET_SPEED / SLOW_DOWN_DISTANCE + SIGN(distance_to_target);

  // Stop motor if target reached and speed is low
  if ((distance_to_target == 0) && (abs(last_target_speed) <= 1)) {
    stop_motor_motion();
    return;
  }
  last_target_speed = target_speed;

  correction_speed = last_error * pid_params[0] / 10;

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

inline int motor_is_idle()
{
  return (motion_mode == IDLE_MODE);
}

inline long get_last_motor_error()
{
  return last_error;
}
