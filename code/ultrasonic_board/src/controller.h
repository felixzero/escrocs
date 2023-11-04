#pragma once

#include <stdbool.h>
#include <stdint.h>

#define ULTRASOUND_TIMEOUT      65535

enum controller_state_t {
    ULTRASOUND_BOARD_IDLE,          /// Nothing to be done
    ULTRASOUND_BOARD_SCANNING,      /// Requires ultrasound scans
    ULTRASOUND_BOARD_UPDATING_LED,  /// Requires LED update
    ULTRASOUND_BOARD_FULL_UPDATE    /// Requires ultrasound scan, then updates
};

/**
 * Tell which channels are to be scanned.
 */
extern bool enabled_channels[];

/**
 * Measured distances from the ultrasound sensors.
 */
extern uint16_t ultrasound_distances[];

/**
 * Configured threshold below which an obstacle is considered critical.
 */
extern uint16_t critical_threshold_distance;

/**
 * Distance in mm below which the LED are displayed green.
 */
extern uint16_t safe_distance;

/**
 * Time in ms between the scan of two adjacent sensors.
 */
extern uint8_t repetition_period;

/**
 * Distance in mm after which the echo is considered absent.
 */
extern uint16_t timeout_distance;

/**
 * Next task to do
 */
enum controller_state_t controller_state;

/**
 * Init the controller subsystem.
 */
void init_controller(void);

/**
 * Returns true if any of the enabled channels detects an obstacle below the critical_threshold_distance.
 */
bool is_path_obstructed(void);

/**
 * Execute the next operation according to the controller_state.
 * Will block until the task is finished.
 */
void run_controller_state_machine(void);

