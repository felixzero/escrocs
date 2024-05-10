#include "steppers.h"
#include "modbus_rtu_slave.h"
#include "i2c_bus.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <math.h>
#define CLAMP_ABS(x, clamp) ((fabsf(x) > (clamp)) ? (clamp) * (x) / fabsf(x) : (x))



int main()
{
    // Set status LED as output
    DDRD |= _BV(2);

    init_steppers();
    init_modbus_rtu_slave();
    //init_i2c();

    while(1) {
        poll_modbus_rtu_operations();
        //poll_i2c_operations();
    }
}
