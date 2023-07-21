#include "i2c.h"
#include "controller.h"
#include "led_strip.h"
#include "ultrasound.h"
#include "modbus.h"

#include <avr/io.h>
#include <avr/delay.h>

int main()
{
    init_controller();
    init_ultrasound();
    init_led_strip();
    init_i2c();

    while (true) {
        perform_ultrasound_scan(poll_i2c_operations);
    }

    //init_modbus();
/*
#if 0
    while (true) {
        send_character('H');
        send_character('e');
        send_character('l');
        send_character('l');
        send_character('o');
        send_character('!');
        send_character('\n');
    }
#else
    DDRB |= _BV(1);
    PORTB &= ~_BV(2);
    while (true) {
        if (poll_character() == 'l') {
            PORTB |= _BV(1);
        } else {
            PORTB &= ~_BV(1);
        }
    }
*/
#endif
}
