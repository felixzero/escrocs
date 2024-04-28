// inspiration : https://github.com/UncleRus/esp-idf-lib/blob/master/components/tca9548/

#pragma once
#include "system/i2c_master.h"

#include <stdbool.h>
#include <esp_err.h>

#define I2C_PORT_TCA9548 I2C_PORT_MOTOR

#define TCA9548_ADDR_0 0x70
#define TCA9548_ADDR_1 0x71
#define TCA9548_ADDR_2 0x72
#define TCA9548_ADDR_3 0x73
#define TCA9548_ADDR_4 0x74
#define TCA9548_ADDR_5 0x75
#define TCA9548_ADDR_6 0x76
#define TCA9548_ADDR_7 0x77

#ifndef BV
#define BV(x) (1 << (x))
#endif

#define TCA9548_CHANNEL0 BV(0)
#define TCA9548_CHANNEL1 BV(1)
#define TCA9548_CHANNEL2 BV(2)
#define TCA9548_CHANNEL3 BV(3)
#define TCA9548_CHANNEL4 BV(4)
#define TCA9548_CHANNEL5 BV(5)
#define TCA9548_CHANNEL6 BV(6)
#define TCA9548_CHANNEL7 BV(7)

esp_err_t tca9548_select_channel(uint8_t channel);
