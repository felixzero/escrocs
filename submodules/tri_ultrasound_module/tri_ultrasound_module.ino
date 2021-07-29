#include "tri_ultrasound.h"

void setup()
{
  Serial.begin(9600);
  //UltraSounds.begin();
  Serial.println("Initiating Ultrasonic test");
}

void loop()
{   
    Serial.println("==============");
    int pin = A0;
    pinMode(pin, OUTPUT);
    int pulsetime = millis();
    digitalWrite(pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin, LOW);
    pinMode(pin, INPUT);
    delayMicroseconds(10);

    Serial.print("A0 > ");
    Serial.println(pulseIn(pin, HIGH) / 58);
 
    pin = A1;
    pinMode(pin, OUTPUT);
    pulsetime = millis();
    digitalWrite(pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin, LOW);
    pinMode(pin, INPUT);
    delayMicroseconds(10);

    Serial.print("A1 > ");
    Serial.println(pulseIn(pin, HIGH) / 58);

    pin = A5;
    pinMode(pin, OUTPUT);
    pulsetime = millis();
    digitalWrite(pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pin, LOW);
    pinMode(pin, INPUT);
    delayMicroseconds(10);

    Serial.print("A5 > ");
    Serial.println(pulseIn(pin, HIGH) / 58);

    delay(1000);
}
