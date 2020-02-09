#include <Wire.h>

#define MAGNETOMETER_I2C_ADDR 0x1E

#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B

#define RAD_TO_DEG 57.296

inline void _write_reg(char reg, char value)
{
  Wire.beginTransmission(MAGNETOMETER_I2C_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

inline uint8_t _read_reg(char reg)
{
  Wire.beginTransmission(MAGNETOMETER_I2C_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(MAGNETOMETER_I2C_ADDR, 1);
  return Wire.read();
}

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  /* Set to lowest caliber */
  _write_reg(CTRL_REG2, 0x00);
  /* Set to high performance mode */
  _write_reg(CTRL_REG4, 0x0C);
}

void loop()
{
  int angle;

  /* Set mode to single measurement and wait until ready */
  _write_reg(CTRL_REG3, 0x01);
  while (!(_read_reg(CTRL_REG3) & 0x02));

  /* Read back heading */
  angle = atan2(_read_reg(OUT_Y_L) | (_read_reg(OUT_Y_H) << 8), _read_reg(OUT_X_L) | (_read_reg(OUT_X_H) << 8)) * RAD_TO_DEG;

  Serial.println(angle);
  delay(100);
}
