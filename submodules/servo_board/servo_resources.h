#pragma once

// Resource usage for the servo board channels
#define SERVO_FLIPPER_LEFT  1
#define SERVO_FLIPPER_RIGHT 2
#define SERVO_FLAG          3
#define SERVO_ARM_LEFT      4
#define SERVO_ARM_RIGHT     5
//#define SERVO_NC 6
//#define SERVO_NC 7
//#define SERVO_NC 8
//#define SERVO_NC 9
//#define SERVO_NC 10
//#define SERVO_NC 11
//#define SERVO_NC 12
//#define SERVO_NC 13

// Syntaxic sugar
#define servo_flag_raise() Servos.writeMicroseconds(SERVO_FLAG, 2000)
#define servo_flag_lower() Servos.writeMicroseconds(SERVO_FLAG, 1000)
