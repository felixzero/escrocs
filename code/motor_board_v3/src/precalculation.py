#!/bin/env python3

import math

with open("src/precalculation.gen.c", "w") as f:
    f.write("/* Generated code, do not commit */\n")
    f.write("#include \"precalculation.h\"\n")
    f.write("#include <avr/pgmspace.h>\n\n")

    f.write("const PROGMEM uint16_t sqrt_diff_precalc[] __attribute__ ((aligned (256))) = {\n")
    for i in range(4096):
        f.write("\t%d,\n" % (32768 * (math.sqrt(i + 1) - math.sqrt(i))))
    f.write("\t255\n")
    f.write("};\n")
