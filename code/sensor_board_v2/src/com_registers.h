#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "com_registers_def.h"

#define RS485_BAUD_RATE         500000

#define MODBUS_SLAVE_ADDR       0x44

#define COM_REG_MOTOR_SPEED_0   10000
#define COM_REG_MOTOR_SPEED_1   10001

#define COM_REG_MOTOR_STEPS_0   10100

#define COM_COIL_ENABLE_MOTORS  20000
