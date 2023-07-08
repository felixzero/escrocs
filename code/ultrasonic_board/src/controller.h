#pragma once

#include <stdbool.h>
#include <stdint.h>

extern bool enabled_channels[];
extern uint8_t ultrasound_distances[];
extern uint8_t critical_threshold_distance;

/* Type of function called as often as the scan can; allowing cooperative multitasking */
typedef void (*yield_fn)(void);

void init_controller(void);
bool is_path_obstructed(void);
void perform_ultrasound_scan(yield_fn yield);
