#include "amalgame.h"
#include "loca_lidar.h"

amalgame_finder_tuning_t amalgame_finder_tuning =  {
    .min_intensity = 5,
    .min_dist = 100,
    .max_dist = 3500,
    .max_distance_betwn_pts = 100,
    .max_amalg_count = 30,
    .max_pt_per_amalg = 20,
};

pose_tuning_t pose_tuning = {
    .max_sq_dist_expected = 40000, //20 cm -> 200^2mm
};


