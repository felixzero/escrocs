//../pinout.h

#pragma once

/* Those are covered by PCINT2 */
#define PIN_SERIAL_RX    0
#define PIN_SERIAL_TX    1
#define PIN_RADIO_INT    2
#define PIN_SERVOS_GSCLK 3
/* Standard US module */
#define PIN_ECHO_FL      4
#define PIN_ECHO_FR      5
#define PIN_ECHO_RL      6
#define PIN_ECHO_RR      7
/* tri US module */
#define PIN_ECHO_FRONT    4
#define PIN_ECHO_120      5
#define PIN_ECHO_240      6

/* Thoses are covered by PCINT0 */
#define PIN_SERVOS_XLAT  8
#define PIN_SERVOS_BLANK 9
#define PIN_SPI_SS       10
#define PIN_SPI_MOSI     11
#define PIN_SPI_MISO     12
#define PIN_SPI_SCK      13

/* Those are covered by PCINT1 */
//#define PIN_NC          A0
//#define PIN_NC          A1
//#define PIN_NC          A2
#define PIN_STARTER     A3
#define PIN_I2C_SDA     A4
#define PIN_I2C_SCL     A5
