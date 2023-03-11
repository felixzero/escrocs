#include "i2c.h"
#include "controller.h"
#include "led_strip.h"
#include "ultrasound.h"

int main()
{
    init_controller();
    init_ultrasound();
    init_led_strip();
    init_i2c();

    while (true) {
        perform_ultrasound_scan(poll_i2c_operations);
    }
}
