#include <Wire.h>
#include <Arduino.h>

#include "lcd_display_controller.h"

#define LCD_I2C_ADDR 0x27

#define LCD_COMMAND_CLEAR_DISPLAY 0x01
#define LCD_COMMAND_RETURN_HOME 0x02
#define LCD_COMMAND_ENTRY_SET_MODE 0x04
#define LCD_COMMAND_DISPLAY_CONTROL 0x08
#define LCD_COMMAND_CURSOR_DISPLAY_SHIFT 0x10
#define LCD_COMMAND_FUNCTION_SET 0x20
#define LCD_COMMAND_SET_CGRAM_ADDR 0x40
#define LCD_COMMAND_SET_DDRAM_ADDR 0x80

#define RS_BIT 0x01
#define RW_BIT 0x02
#define ENABLE_BIT 0x04
#define BACKLIGHT_BIT 0x08

#define SECOND_RAW_RAM_OFFSET 0x40
#define MAX_LENGTH 16

#define POWER_UP_MS 50
#define DELAY_INIT_1_US 4100
#define DELAY_INIT_2_US 100
#define DELAY_AFTER_COMMAND_US 37
#define DELAY_AFTER_HOMING_US 1520
#define ENABLE_PULSE_US 1

LcdDisplayController LcdDisplay;

void LcdDisplayController::init() {
  delay(POWER_UP_MS);

  // Initialization procedure and switch to 4 bits
  sendRawCommand(0x03, 0x00);
  delayMicroseconds(DELAY_INIT_1_US);
  sendRawCommand(0x03, 0x00);
  delayMicroseconds(DELAY_INIT_2_US);
  sendRawCommand(0x03, 0x00);
  sendRawCommand(0x02, 0x00);

  // Static configuration
  sendCommand(LCD_COMMAND_FUNCTION_SET | B01000);
  sendCommand(LCD_COMMAND_DISPLAY_CONTROL | B100);
  sendCommand(LCD_COMMAND_CLEAR_DISPLAY);
  sendCommand(LCD_COMMAND_ENTRY_SET_MODE | B10);
  sendCommand(LCD_COMMAND_RETURN_HOME);
  delayMicroseconds(DELAY_AFTER_HOMING_US);

  writeString("Score: ---");
}

void LcdDisplayController::showScore(int score) {
  moveToPosition(7, 0);
  sendData('0' + (score / 100) % 10);
  sendData('0' + (score / 10) % 10);
  sendData('0' + score % 10);
}

void LcdDisplayController::displayMessage(char *message) {
  moveToPosition(0, 1);
  writeString(message);
}

void LcdDisplayController::moveToPosition(uint8_t col, uint8_t row) {
  sendCommand(LCD_COMMAND_SET_DDRAM_ADDR | (col + SECOND_RAW_RAM_OFFSET * row));
}

void LcdDisplayController::writeString(char *message) {
  int i = 0;
  
  for (; message[i] != NULL && i < MAX_LENGTH; ++i) {
    sendData(message[i]);
  }

  for (int j = i; j < MAX_LENGTH; ++j) {
    sendData(' ');
  }
}

void LcdDisplayController::sendCommand(uint8_t command) {
  sendRawCommand((command & 0xF0) >> 4, 0);
  sendRawCommand(command & 0x0F, 0);
  delayMicroseconds(DELAY_AFTER_COMMAND_US);
}

void LcdDisplayController::sendData(uint8_t data) {
  sendRawCommand((data & 0xF0) >> 4, RS_BIT);
  sendRawCommand(data & 0x0F, RS_BIT);
  delayMicroseconds(DELAY_AFTER_COMMAND_US);
}

void LcdDisplayController::sendRawCommand(uint8_t command, uint8_t flags) {
  uint8_t byteToSend = (command << 4) | (flags & 0xF) | BACKLIGHT_BIT;
  
  writeByteOverI2C(byteToSend);
  writeByteOverI2C(byteToSend | ENABLE_BIT);
  delayMicroseconds(ENABLE_PULSE_US);
  writeByteOverI2C(byteToSend & ~ENABLE_BIT);
}

void LcdDisplayController::writeByteOverI2C(uint8_t byteToSend) {
  Wire.beginTransmission(LCD_I2C_ADDR);
  Wire.write(byteToSend);
  Wire.endTransmission();
}
