#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>

#include "load_compensated_motors.h"
#include "basic_io.h"
#include "i2c.h"

int main(void)
{
    init_load_compensated_motors();
    init_basic_io();
    init_i2c();
    set_bldc_speed(150);
    set_bldc_direction(true);
    set_load_compensated_motor_speed_target(0, 20);
    set_load_compensated_motor_speed_target(1, 20);
    set_power_io_flags(0xFF);

    DDRB |= _BV(0);
    PORTB |= _BV(0);

    while (true) {
        poll_i2c_operations();
    }
}
