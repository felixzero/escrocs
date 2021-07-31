#pragma once

#include "servos.h"

// Resource usage for the servo board channels
#define SERVO_FLIPPER_RIGHT 4
#define SERVO_FLIPPER_LEFT  5
#define SERVO_FLAG          6
#define SERVO_ARM_LEFT      7
#define SERVO_ARM_MIDDLE    8
#define SERVO_ARM_RIGHT     9
#define SERVO_PINCET_LEFT   10
#define SERVO_PINCET_MIDDLE 11
#define SERVO_PINCET_RIGHT  12
//#define SERVO_NC 13

// Syntaxic sugar
#define SERVO_FLAG_RAISE() Servos.writeMicroseconds(SERVO_FLAG, 2000)
#define SERVO_FLAG_LOWER() Servos.writeMicroseconds(SERVO_FLAG, 1000)

#define SERVO_FLIPPER_LEFT_OPEN() Servos.writeMicroseconds(SERVO_FLIPPER_LEFT, 1000)
#define SERVO_FLIPPER_LEFT_CLOSE() Servos.writeMicroseconds(SERVO_FLIPPER_LEFT, 2000)

#define SERVO_FLIPPER_RIGHT_OPEN() Servos.writeMicroseconds(SERVO_FLIPPER_RIGHT, 2000)
#define SERVO_FLIPPER_RIGHT_CLOSE() Servos.writeMicroseconds(SERVO_FLIPPER_RIGHT, 1000)

#define ARM_LEFT   0
#define ARM_MIDDLE 1
#define ARM_RIGHT  2
#define SERVO_ARM_RAISE(which) Servos.writeMicroseconds(SERVO_ARM_LEFT + which, 2000)
#define SERVO_ARM_LOWER(which) Servos.writeMicroseconds(SERVO_ARM_LEFT + which, 1000)
#define SERVO_PINCET_OPEN(which) Servos.writeMicroseconds(SERVO_PINCET_LEFT + which, 1000)
#define SERVO_PINCET_CLOSE(which) Servos.writeMicroseconds(SERVO_PINCET_LEFT + which, 2000)
