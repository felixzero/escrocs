#include "holonomic_feedback.h"

#include <Arduino.h>

void setup() {
    init_holonomic_feedback(0.0, 0.0, 0.0);
    set_holonomic_feedback_target(-7600.0, 12800.0, 0.0);
}

void loop() {
    holonomic_feedback_loop();
    delay(10);
}
