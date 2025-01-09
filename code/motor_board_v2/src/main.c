#include <avr/io.h>
#include <stdbool.h>

#include "motor_pwm.h"
#include "encoder.h"
#include "i2c_bus.h"

int main(void)
{
    // Set status LED as output
    DDRB |= _BV(0);

    // Init all subsystems
    init_motor_pwm();
    init_encoder();
    init_i2c();

    while(true) {
        poll_i2c_operations();
    }
}
