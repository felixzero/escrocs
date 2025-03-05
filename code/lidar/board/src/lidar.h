#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "../loca_lidar/amalgame.h"
#include "../loca_lidar/loca_lidar.h"

extern int nb_amalg;
extern amalgame_t* full_amalgames;
extern QueueHandle_t polar_array_queue;

typedef struct {
    polar_t pts[MAX_AMALG_COUNT];
    uint16_t length;
} polar_array_t;


void init_uart();