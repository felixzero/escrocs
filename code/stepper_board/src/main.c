#include "pumps.h"
#include "steppers.h"
#include "i2c_bus.h"

#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

int main()
{
    init_i2c();
    init_pumps();
    init_steppers();

    DDRD |= _BV(2);

    while (true) {
        poll_i2c_operations();
    }
}
