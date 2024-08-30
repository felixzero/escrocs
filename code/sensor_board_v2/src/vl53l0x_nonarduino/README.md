# VL53L0X non-Arduino library

Version: 1.0.2<br>
Release date: 2017 Jun 27<br>

## Summary

This is a lightweight C library that helps to interface with ST's VL53L0X time-of-flight distance sensor. It makes it simple to configure the sensor and read range data from it via I&sup2;C.

I converted Pololus C++ Arduino code to a Makefile based C project. The original can be found [here](https://github.com/pololu/vl53l0x-arduino).

While I was able to get ST's original API running, it used > 26 kB (of 32 kB) of program memory. This library, while having somewhat reduced functionality, uses only about 4 kB.

## Supported platforms

This library is designed to work with AVR-GCC and has been tested with an Arduino Pro Mini (without the Arduino IDE). The library comes with a running example. That means, you'll get a complete development environment here. There's a Makefile, some slim UART printing functions, a complete set of I2C master communication functions and my very own implementation of the `millis()` function (shamelessly stolen from the Arduino lib.).

## Getting started
See the notes of the [original C++ Arduino library](https://github.com/pololu/vl53l0x-arduino).

## Examples
Connect an Arduino Pro Mini to `/dev/ttyUSB0` and ...

```C
  make bootload
```

This will compile, flash (avrdude) and open a terminal window for debugging (miniterm).
