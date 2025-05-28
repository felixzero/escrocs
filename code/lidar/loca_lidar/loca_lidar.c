#include "amalgame.h"
#include "loca_lidar.h"

amalgame_finder_tuning_t amalgame_finder_tuning =  {
    .min_intensity = 50,
    .min_dist = 100, //mm //TODO : voir pour inhiber moins 
    .max_dist = 3800,//3800,
    .max_distance_betwn_pts = 100,
    .max_amalg_count = MAX_AMALG_COUNT,
    .max_pt_per_amalg = 20,
};

pose_tuning_t pose_tuning = {
    .max_sq_dist_expected = 1000, //3 cm -> 30^2 mm
    .max_sq_dist_large_expected = 250000,//90000, //30cm 300^2mm
    .max_dist_beacons = 50, //mm

};


