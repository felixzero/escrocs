/*
    USDM - Ultra-Sonic Detection Module
   --------------------------------------------
    Management of 6 HC-SR04 US modules,
    Visual feedback through WS2812B LED ring
   --------------------------------------------
    E.S.C.RO.C.S 2021-2022
*/

#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#include "ultrasound.h"
#include "pinout.h"

#define LED_COUNT 24 // TBC - assuming HALO Ring

#define I2C_ADDR 04

#define CMD_SET_MODE 0  // setmode (mode, args)
#define CMD_READ_USM 1  // read (channel)
#define CMD_READ_ALL 2  // read (channels 1 through 6)
#define CMD_SAFE_DIR 3  // is_direction_safe (int azimuth (°))
#define CMD_SET_SIDE 4  // set the side color to display in wait() state

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
//   Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

uint32_t black = strip.Color(0, 0, 0);
uint32_t red = strip.Color(255, 0, 0);
uint32_t green = strip.Color(0, 255, 0);
uint32_t colorRamp[] = { black, red, red, red, red, red, strip.Color(255, 64, 0), strip.Color(255, 128, 0), strip.Color(255, 255, 0), strip.Color(128, 255, 0), strip.Color(64, 255, 0), strip.Color(0, 255, 0)};

uint32_t sideYellow = strip.Color(0xFF, 0x3F, 0x00); //(0xFF, 0x2F, 0x00);
uint32_t sidePurple=  strip.Color(0x62, 0x00, 0xDF); //(0x92, 0x00, 0xDF); 

uint32_t currentSide = sideYellow;
uint32_t lastSide = -1;

// Mode enum for controlling the display. Mode change is requested through I2C by the master.
enum ModuleMode
{
  Startup = 0,
  Wait = 1,
  Detection = 2,
  Endgame = 3,
  Error = 4
};

ModuleMode _mode = ModuleMode::Startup;

int _safeDistance = 30; // cm

void setup()
{
  Serial.begin(9600);
  // Wire setup for I2C --------------------------------------------------
  Wire.begin(I2C_ADDR);
  Wire.onReceive(i2cCallback);
  // US Modules setup  ---------------------------------------------------
  UltraSounds.begin();
  // NeoPixel Setup ------------------------------------------------------
  strip.begin();           // INITIALIZE NeoPixel strip object
  strip.show();            // Turn OFF all pixels
  strip.setBrightness(40); // Set BRIGHTNESS to about 1/6 (max = 255)
  //----------------------------------------------------------------------
  pinMode(PIN_SWITCH, INPUT_PULLUP); // this should be removed after testing is done

  Serial.println("USDM ready to go !!!");
}

// Read sonar distances and update LED display based on mode and distance
void loop()
{
  //Pulse all sonars to refresh data
  UltraSounds.pulse(ULTRASOUND_0);
  delayMicroseconds(80000);
  UltraSounds.pulse(ULTRASOUND_60);
  delayMicroseconds(80000);
  UltraSounds.pulse(ULTRASOUND_120);
  delayMicroseconds(80000);
  UltraSounds.pulse(ULTRASOUND_180);
  delayMicroseconds(80000);
  UltraSounds.pulse(ULTRASOUND_240);
  delayMicroseconds(80000);
  UltraSounds.pulse(ULTRASOUND_300);
  delayMicroseconds(80000);

  int det1 = UltraSounds.read(ULTRASOUND_0);
  int det2 = UltraSounds.read(ULTRASOUND_60);
  int det3 = UltraSounds.read(ULTRASOUND_120);
  int det4 = UltraSounds.read(ULTRASOUND_180);
  int det5 = UltraSounds.read(ULTRASOUND_240);
  int det6 = UltraSounds.read(ULTRASOUND_300);

  int value = digitalRead(PIN_SWITCH);
  _mode = value ? Endgame : Wait;  

  switch (_mode)
  {
    case Detection: // standard operating state.
    {
      ////Serial.println("Detection mode!!!");
      // Pulse each sonar, display the corresponding distance as colored arc
      // NOPE -- write all the results on the serial port for logging -- NOPE
      setColorFromDistance(1, det1);
      setColorFromDistance(2, det2);
      setColorFromDistance(3, det3);
      setColorFromDistance(4, det4);
      setColorFromDistance(5, det5);
      setColorFromDistance(6, det6);
      strip.show();
      break;
    }
    case Startup:
    {
      ////Serial.println("Startup mode!!!");
      //animate start();
      // Pulse each module once and check if all US modules are responding correctly.
      // Display a green arc under OK modules and red under not responding ones.
      det1 > 0 ? strip.fill(strip.Color(0, 128, 0),  0, 4) : strip.fill(strip.Color(128, 0, 0), 0, 4);
      det2 > 0 ? strip.fill(strip.Color(0, 128, 0),  4, 4) : strip.fill(strip.Color(128, 0, 0), 4, 4);
      det3 > 0 ? strip.fill(strip.Color(0, 128, 0),  8, 4) : strip.fill(strip.Color(128, 0, 0), 8, 4);
      det4 > 0 ? strip.fill(strip.Color(0, 255, 0), 12, 4) : strip.fill(strip.Color(128, 0, 0), 12, 4);
      det5 > 0 ? strip.fill(strip.Color(0, 128, 0), 16, 4) : strip.fill(strip.Color(128, 0, 0), 16, 4);
      det6 > 0 ? strip.fill(strip.Color(0, 128, 0), 20, 4) : strip.fill(strip.Color(128, 0, 0), 20, 4);
      strip.show();
      break;
    }
    case Wait:
    {
      ////Serial.println("Startup mode!!!");
      //While waiting for the game to start, display estimated side color (read from I2C)
      if(currentSide!=lastSide)
      {
        colorWipe(currentSide, 10);
        lastSide = currentSide;
      }
      else
        rotateVoid(currentSide, 40);

      delay(50);
      break;
    }
    case Endgame:
    {
      //WARNING: Blocking call
      rainbow(10, 1);
      break;
    }
    case Error:
    {
      //flash ERROR();
      theaterChase(strip.Color(127,   0,   0), 50);
      break;
    }
    default:
      Serial.println("Erreur de mode!!! Passage a ERREUR.");
      _mode = Error;
      break;
  }
}

/* ----------------------------------------------
 *  THIS CODE SHOULD BE MOVED TO OWN SUBCLASS
 * ----------------------------------------------
 */
// TODO: better interpolation
void setColorFromDistance(int module, int distance)
{
  uint32_t color;
  color = distance > 550 ? strip.Color(0, 255, 255) : colorRamp[ (int)(distance + 1) / 50 ];
  distance > 0 ? strip.fill(color,  (module - 1) * 4, 4) : strip.fill(strip.Color(255, 0, 0), (module - 1) * 4, 4);
}


void rainbow(int wait, int cycles) {
  // Hue of first pixel runs _cycles_ complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < cycles * 65536; firstPixelHue += 256) {
    for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
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
/* ----------------------------------------------
 *  END COLOR CODE
 * ----------------------------------------------
 */


void i2cCallback(int howMany) {
  while (howMany >= 2) {
    manageI2cCommand(Wire.read(), Wire.read());
    howMany -= 2;
  }
}

// WIP
void manageI2cCommand(uint8_t command, uint8_t argument) {
  uint8_t queryChannel = command & 0x3;
  uint8_t queryCommand = command >> 2;

  switch (queryCommand) {
    case CMD_SET_MODE:
      _mode = static_cast<ModuleMode>(queryChannel);
      // On transition to startup, (re-)initialize the SAFE_DISTANCE parameter
      if (_mode == ModuleMode::Startup)
      {
        _safeDistance = argument;
      }
      if (_mode == ModuleMode::Wait)
      {
        lastSide = currentSide;
        currentSide = (argument) ? sideYellow : sidePurple;
      }
      //writeLong(Encoder.readChannel(queryChannel));
      break;

    case CMD_READ_USM:
      //Motor.writeSpeed(queryChannel, argument);
      //writeLong(Encoder.readChannel(queryChannel));
      break;

    //used to change side color without altering state (supposedly while already in  Wait state).
    // Convenience function; SET_MODE (WAIT, new color) should work too
    case CMD_SET_SIDE:
      lastSide = currentSide;
      currentSide = (argument) ? sideYellow : sidePurple;
      break;

    default:
      //writeLong(0xFFFFFFFF);
      ;
  }
}
