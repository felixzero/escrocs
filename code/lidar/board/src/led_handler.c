#include "led_handler.h"

CRGB* ws2812_buffer;

void init(uint8_t pin, uint8_t nb_led, float angle_offset) {
    //init task
}
void led_handler_task(void *pvParameters) {
    //for(nb led)
    //ws2812_buffer[i] = (CRGB){.r=0, .g=0, .b=0};
}
//get proximity angle & value
//get beacon
//Check if  "rest" color set by I2C
//MAX_PRIMARY_COLOR_VALUE
//Find number of leds
//Update them
