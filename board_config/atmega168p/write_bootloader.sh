#!/bin/sh

avrdude -c usbasp -p m168p -U lfuse:w:0xFF:m -U hfuse:w:0xDE:m -U efuse:w:0xFD:m -U flash:w:optiboot_atmega168.hex:i

