#!/bin/sh

avrdude -c usbasp -p m328pb -U lfuse:w:0xFF:m -U hfuse:w:0xD9:m -U efuse:w:0xFE:m
