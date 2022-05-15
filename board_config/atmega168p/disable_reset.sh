#!/bin/sh

avrdude -c usbasp -p m168p -U hfuse:w:0x5E:m
