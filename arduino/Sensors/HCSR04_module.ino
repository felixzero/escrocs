
#define TRIG 2
#define ECHO1 3
#define ECHO2 4

void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

int echoStatus1 = LOW;
int echoStatus2 = LOW;

int startTime1, endTime1, startTime2, endTime2;

ISR (PCINT2_vect)
{
  int newStatus1 = digitalRead(ECHO1);
  int newStatus2 = digitalRead(ECHO2);

  if ((newStatus1 == HIGH) && (echoStatus1 != newStatus1)) { // Rising 1
    startTime1 = micros();
  }
  if ((newStatus1 == LOW) && (echoStatus1 != newStatus1)) { // Falling 1
    endTime1 = micros();
  }
  if ((newStatus2 == HIGH) && (echoStatus2 != newStatus2)) { // Rising 2
    startTime2 = micros();
  }
  if ((newStatus2 == LOW) && (echoStatus2 != newStatus2)) { // Falling 2
    endTime2 = micros();
  }

  echoStatus1 = newStatus1;
  echoStatus2 = newStatus2;
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(ECHO2, INPUT);
    
  digitalWrite(TRIG, LOW);  
  delayMicroseconds(500000);

  pciSetup(ECHO1);
  pciSetup(ECHO2);
}

void loop() {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
//  int pulseLenght = pulseIn(ECHO2, HIGH, 60000);
//  Serial.println(pulseLenght*0.34/2);
  
  delayMicroseconds(60000);

  Serial.print("1> ");
  Serial.print((endTime1 - startTime1)*0.34/2);
  Serial.print("2> ");
  Serial.println((endTime2 - startTime2)*0.34/2); 

  /*if ((endTime1 - startTime1)*0.34/2 < 1000)
    Serial.print(" [1] ");

  if ((endTime2 - startTime2)*0.34/2 < 1000)
    Serial.print(" [2] ");*/

  Serial.println();
}
