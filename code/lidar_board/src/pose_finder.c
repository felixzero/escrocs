#include "pose_finder.h"

#define BEACON_REFLECTIVITY_BEACON

pose_t find_pose(const uint16_t *distances, const uint16_t *intensities)
{
    pose_t pose = {
        .x = 1.0,
        .y = 0.0,
        .theta = 0.0
    };

    for (int i = 0; i < NUMBER_OF_ANGLES; ++i) {

    }

    return pose;
}
