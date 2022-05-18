#include "Arduino.h"
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

#define LED_COUNT 60
#define LED_PIN 6
#define SERVO_PIN 7
#define CONTACT_PIN A2
#define SIDE_PIN A3

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

Servo arm; 
int done = 0;

uint32_t sideYellow = strip.Color(0xFF, 0x3F, 0x00); //(0xFF, 0x2F, 0x00);
uint32_t sidePurple=  strip.Color(0x62, 0x00, 0xDF); //(0x92, 0x00, 0xDF);
uint32_t currentSide = sideYellow;


void fetch() // Move the servo gently, to avoid detaching the magnets
{
  //Serial.println("FETCHING =====================");
  int servoAngle = 90; 
  for (servoAngle = 90; servoAngle > 0; servoAngle--)
    {
      arm.write(servoAngle);
      Serial.println(servoAngle);
      delay(10);
    }
}


void rainbow(int wait, int cycles) {
  // Hue of first pixel runs _cycles_ complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < cycles * 65536; firstPixelHue += 256) 
  {
    for (int i = 0; i < strip.numPixels(); i++)
    { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

void rotateVoid(uint32_t color, int wait) {
  for(int i=1; i<strip.numPixels()+1; i++) { 
    strip.setPixelColor(i-1, color); 
    strip.setPixelColor(i%strip.numPixels(), strip.Color(0,0,0));            
    strip.show();                         
    delay(wait);                           
  }
}

void setup() 
{
  Serial.begin(9600);
  pinMode(CONTACT_PIN, INPUT_PULLUP); // contact switch  
  pinMode(SIDE_PIN, INPUT_PULLUP);    // side switch
  arm.attach(SERVO_PIN);
  arm.write(89); // init pos
  
  // NeoPixel Setup ------------------------------------------------------
  strip.begin();           // INITIALIZE NeoPixel strip object
  strip.show();            // Turn OFF all pixels
  strip.setBrightness(40); // Set BRIGHTNESS to about 1/6 (max = 255)
  //----------------------------------------------------------------------

  currentSide = digitalRead(SIDE_PIN) ? sideYellow : sidePurple; 
  Serial.print("Side: ");
  Serial.print(currentSide);
  Serial.print(" / ");
  Serial.println(digitalRead(SIDE_PIN));
}

void loop()
{
  int contact = digitalRead(CONTACT_PIN);
  Serial.print("Switch: ");
  Serial.print(contact);
  Serial.print(" / ");
  Serial.println(digitalRead(CONTACT_PIN));
  
  Serial.print("Done: ");
  Serial.println(done);
  
  if(!done)
  { //Update side color, just in case
    currentSide = digitalRead(SIDE_PIN) ? sideYellow : sidePurple; 
    Serial.print(currentSide);
    Serial.println(digitalRead(SIDE_PIN));
  }


  if(contact && !done)
  {
    done = 1;
    // We got contact !
    colorWipe(currentSide, 10); // total wait ~600ms
    rotateVoid(currentSide, 5); // total wait ~300ms
    
    delay(1100); //Wait 2s for robot to lower claw, in order to clear the hook
    
    // ITS PLACE IS IN A MUSEUM !!!
    fetch();
  }
  if(done)
  {
    //Light the fireworks
    theaterChase(currentSide, 50);
    theaterChase(currentSide, 50);
    rainbow(10, 3);
    theaterChase(currentSide, 50);
  }
  
  delay(200);
}
