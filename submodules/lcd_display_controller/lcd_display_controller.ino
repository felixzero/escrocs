#include "lcd_display_controller.h"

void setup() {
  Serial.begin(9600);
  
  LcdDisplay.init();
  LcdDisplay.showScore(64);
  LcdDisplay.displayMessage("Match termine");
}

void loop() {
  // nothing to loop
}
