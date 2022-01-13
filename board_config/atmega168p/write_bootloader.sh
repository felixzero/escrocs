#!/bin/sh

avrdude -c stk500v1 -p m168p -P /dev/ttyUSB0 -b 19200 -U lfuse:w:0xFF:m -U hfuse:w:0xDE:m -U efuse:w:0xFD:m -U flash:w:optiboot_atmega168.hex:i

