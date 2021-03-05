#include "motor.h"
#include "encoder.h"

void setup() {
    Motor.begin();
    Encoder.begin();
    Serial.begin(9600);
}

void loop() {
    goBaby(MotorSingleton::Mode::ClockWise);
    goBaby(MotorSingleton::Mode::CounterClockWise);
}

void goBaby(MotorSingleton::Mode mode) {
    Motor.writeChannel(0, mode, 250);
    Motor.writeChannel(1, mode, 250);
    for (int i = 0; i < 1000; ++i) {
            long encoder = Encoder.readChannel(1);
            Serial.print("Encoder read: ");
            Serial.println(encoder);
            delay(10);
    }
}
