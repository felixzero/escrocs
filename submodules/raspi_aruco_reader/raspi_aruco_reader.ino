#include "wind_info.h"

const char inputPin = 2;
const char inputPin2 = 3;

void setup() {
  Serial.begin(9600);
  WindInfos.begin(inputPin, inputPin2);

}

void loop() {
  int southDetector = 0;
  int unreliable    = 0;
  for(int i=0; i< 5; i++)
  {
    WindInfo::Status state = WindInfos.detectWind(true);
    if (state == WindInfo::South) {
      southDetector++;
    }
    if (state == WindInfo::Undetected) {
      unreliable++;
    }
    delay(100);
    Serial.print(state);
    Serial.print("; ");
  }
  Serial.println();
  Serial.println((southDetector>=3)?("SOUTH detected"):("NORTH (or unknown)"));
  delay(1000);
}
