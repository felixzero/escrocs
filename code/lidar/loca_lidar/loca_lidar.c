#include "amalgame.h"
#include "loca_lidar.h"

amalgame_finder_tuning_t amalgame_finder_tuning =  {
    .min_intensity = 5,
    .min_dist = 100,
    .max_dist = 3800,
    .max_distance_betwn_pts = 100,
    .max_amalg_count = 50,
    .max_pt_per_amalg = 20,
};

pose_tuning_t pose_tuning = {
    .max_sq_dist_expected = 1000, //3 cm -> 30^2 mm
    .max_sq_dist_large_expected = 90000, //30cm 300^2mm
    .max_dist_beacons = 120, //120mm

};


