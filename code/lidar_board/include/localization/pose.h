#pragma once

#include <stdint.h>

#define NUMBER_OF_ANGLES 360

typedef struct {
    float x;
    float y;
    float theta;
} pose_t;

typedef struct {
    float x;
    float y;
} point_t;
