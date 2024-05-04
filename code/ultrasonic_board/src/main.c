#include "controller.h"
#include "led_strip.h"
#include "ultrasound.h"
#include "modbus_rtu_slave.h"
#include "i2c_bus.h"

#include <avr/io.h>

int main()
{
    init_controller();
    init_ultrasound();
    init_led_strip();
    //init_modbus_rtu_slave();
    init_i2c();

    while (true) {
        //poll_modbus_rtu_operations();
        while (poll_i2c_operations());
        run_controller_state_machine();
    }
}
