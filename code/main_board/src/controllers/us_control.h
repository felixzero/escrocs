#pragma once

#include "peripherals/ultrasonic_board.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>


#define ULTRASONIC_CHANNEL_PERIOD_MS 30

typedef struct {
    float center_angle;
    float cone;
} scan_angle_t;

extern QueueHandle_t strategy_single_channel_queue, motion_cone_queue, scan_over_queue;
extern uint16_t distances[NUMBER_OF_US];

esp_err_t init_us_controller();

// Can be used to know how long to wait with ULTRASONIC_CHANNEL_PERIOD_MS * get_number_of_active_channels
int get_number_of_active_channels(void);
