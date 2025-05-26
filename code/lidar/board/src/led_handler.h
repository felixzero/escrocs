#pragma once

#include "stdint.h"
#include "esp_ws28xx.h"

#define LED_GPIO 13
#define MAX_PRIMARY_COLOR_VALUE 50

void init(uint8_t pin, uint8_t nb_led, float angle_offset);
void led_handler_task(void *pvParameters);
