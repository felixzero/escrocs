#!/bin/sh

avrdude -c usbasp -p m328p -U lfuse:w:0xE0:m -U hfuse:w:0xD9:m -U efuse:w:0xFE:m
