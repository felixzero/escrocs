#include "steppers.h"
#include "modbus_rtu_slave.h"

#include <avr/io.h>

int main()
{
    // Set status LED as output
    DDRD |= _BV(2);

    init_steppers();
    init_modbus_rtu_slave();

    while(1) {
        poll_modbus_rtu_operations();
    }
}
