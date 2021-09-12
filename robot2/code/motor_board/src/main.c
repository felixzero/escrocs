#include "holonomic_feedback.h"
#include "i2c_slave.h"

#include <Arduino.h>

void setup() {
    init_holonomic_feedback(0.0, 0.0, 0.0);
    init_i2c();
}

void loop() {
    static unsigned long last_millis = 0;
    poll_i2c_operations();

    if (millis() > last_millis + 10) {
        holonomic_feedback_loop();
        last_millis = millis();
    }
}
