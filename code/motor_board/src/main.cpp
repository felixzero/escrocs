extern "C" {
#include "motor.h"
#include "encoder.h"
#include "i2c_slave.h"
}

#include <Arduino.h>

void setup() {
    Serial.begin(9600);

    init_encoders();
    init_motors();
    write_motor_speed(0, 0, 0);

    init_i2c();
}

void loop() {
    poll_i2c_operations();
}
