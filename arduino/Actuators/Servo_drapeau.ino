#include <Servo.h>
 
Servo flag_servo;  // crée l’objet pour contrôler le servomoteur
 
void setup()
{
  flag_servo.attach(9);  // utilise la broche 9 pour le contrôle du servomoteur
  flag_servo.write(25); // positionne le servomoteur à 0°
  delay(10000);
}
 
 
void loop()
{
  Serial.println("Start loop");
  Serial.print(flag_servo.read());
   flag_servo.write(70);
   delay(150);
   flag_servo.write(130);
   delay(5000);
   flag_servo.write(70);
   delay(150);
   flag_servo.write(28);
  delay(5000);
}
