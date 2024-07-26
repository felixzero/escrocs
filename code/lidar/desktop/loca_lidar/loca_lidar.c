#include "amalgame.h"
#include "loca_lidar.h"

amalgame_finder_tuning_t amalgame_finder_tuning =  {
    .min_intensity = 5,
    .min_dist = 50,
    .max_dist = 3500,
    .max_distance_betwn_pts = 100,
    .max_amalg_count = 25,
    .max_pt_per_amalg = 20,
};


