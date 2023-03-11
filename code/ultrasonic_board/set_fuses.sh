#!/bin/sh

avrdude -c usbasp -p m8 -U lfuse:w:0xbf:m -U hfuse:w:0xd9:m

