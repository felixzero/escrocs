#pragma once

#include <stdint.h>

#define NUMBER_OF_ANGLES 360

typedef struct {
    float x;
    float y;
    float theta;
} pose_t;

pose_t find_pose(const uint16_t *distances, const uint16_t *intensities);
