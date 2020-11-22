#pragma once

#include "servos.h"

// Resource usage for the servo board channels
#define SERVO_FLIPPER_RIGHT 4
#define SERVO_FLIPPER_LEFT  5
#define SERVO_FLAG          6
//#define SERVO_ARM_LEFT      7
//#define SERVO_ARM_RIGHT     8
//#define SERVO_NC 9
//#define SERVO_NC 10
//#define SERVO_NC 11
//#define SERVO_NC 12
//#define SERVO_NC 13

// Syntaxic sugar
#define SERVO_FLAG_RAISE() Servos.writeMicroseconds(SERVO_FLAG, 2000)
#define SERVO_FLAG_LOWER() Servos.writeMicroseconds(SERVO_FLAG, 1000)

#define SERVO_FLIPPER_LEFT_OPEN() Servos.writeMicroseconds(SERVO_FLIPPER_LEFT, 1000)
#define SERVO_FLIPPER_LEFT_CLOSE() Servos.writeMicroseconds(SERVO_FLIPPER_LEFT, 2000)

#define SERVO_FLIPPER_RIGHT_OPEN() Servos.writeMicroseconds(SERVO_FLIPPER_RIGHT, 2000)
#define SERVO_FLIPPER_RIGHT_CLOSE() Servos.writeMicroseconds(SERVO_FLIPPER_RIGHT, 1000)
