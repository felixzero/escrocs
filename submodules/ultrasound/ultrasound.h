#pragma once

#define ULTRASOUND_READ_FL 0
#define ULTRASOUND_READ_FR 1
#define ULTRASOUND_READ_RL 2
#define ULTRASOUND_READ_RR 3

#define ULTRASOUND_PULSE_FL (1 << ULTRASOUND_READ_FL)
#define ULTRASOUND_PULSE_FR (1 << ULTRASOUND_READ_FR)
#define ULTRASOUND_PULSE_RL (1 << ULTRASOUND_READ_RL)
#define ULTRASOUND_PULSE_RR (1 << ULTRASOUND_READ_RR)
#define ULTRASOUND_PULSE_FRONT (ULTRASOUND_PULSE_FL | ULTRASOUND_PULSE_FR)
#define ULTRASOUND_PULSE_REAR (ULTRASOUND_PULSE_RL | ULTRASOUND_PULSE_RR)

/* Initialize modules */
void ultrasound_init();

/* Send ultrasound pulse to a specific set of emitters */
void ultrasound_pulse(char module_selection_flag);

/*  Returns the distance in mm to the obstacle, read by a specific module.
 *  Returns -1 if no data is available */
int ultrasound_read(char module);
