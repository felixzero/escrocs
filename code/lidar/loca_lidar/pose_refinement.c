#include "pose_refinement.h"

#include "amalgame.h"
#include "math.h"
#include "stdbool.h"

#define MAX_COMBINATIONS 100

pose_t estimated_lidar = { // Actually robot pose
    .angle_rad = 1.48,
    .pos.x = 1680,
    .pos.y = 1020
};
pose_t empty_pose = {
    .angle_rad = -1.0,
    .pos.x = -1.0,
    .pos.y = -1.0
};

pose_t too_many_corr_pose = {
    .angle_rad = -1.0,
    .pos.x = -2.0,
    .pos.y = -1.0
}; //Used for error handling

pose_t too_many_candidates_pose = {
    .angle_rad = -1.0,
    .pos.x = -3.0,
    .pos.y = -1.0
}; //Used for error handling

//use generate_sq_dist_beacons.py
//uint32_t distances_beacons[MAX_NB_BEACONS][MAX_NB_BEACONS] = {
//    {0, 11065844, 3610000}, {0, 0, 11065844}, {0, 0, 0},
//};//COmment faire distances beacons ??
float distances_beacons[MAX_NB_BEACONS][MAX_NB_BEACONS] = {
    {0, 3326.5f, 1900.f}, {0, 0, 3326.5f}, {0, 0, 0},
};

static float sq_dist(point_t a, point_t b);
static bool in_range(float dist, float expected_dist, float err_tolerance);

pose_t refined_lidar;
uint16_t last_assos[MAX_NB_BEACONS];
uint8_t indexs_debug[MAX_CANDIDATES_BEACONS];
uint8_t many_corr = 1; //Used to know if multiple correspondances of maximum length has been found (=> 0)

void convert_xy(point_t* pts, uint16_t count, const uint16_t angles[], const uint16_t distances[]) {
    for (uint16_t i = 0; i < count; i++)
    {
        pts[i].x = (int32_t) (distances[i] * cos(CENTIDEGREES_TO_RADIANS(angles[i])));
        pts[i].y = (int32_t) (distances[i] * sin(CENTIDEGREES_TO_RADIANS(angles[i])));
    }
}

void init_distances() {
    for (size_t i = 0; i < MAX_NB_BEACONS; i++)
    {
        //distances_beacons[i] = 
    }
    
}
void set_estimated_pose(int32_t x, int32_t y, float angle) {
    estimated_lidar.pos.x = x;
    estimated_lidar.pos.y = y;
    estimated_lidar.angle_rad = angle;
}

pose_t get_refined_lidar() {
    return refined_lidar;
}
pose_t refine_pose(point_t* candidates, amalgame_t* amalgames, uint16_t nb_amalgs, 
    pose_t estimated_odom, const pose_tuning_t* tuning) {
    if (estimated_odom.pos.x == 0 && estimated_odom.pos.y == 0) {
        estimated_odom = estimated_lidar;
        if(estimated_lidar.pos.x == 0 && estimated_lidar.pos.y == 0) {
            return empty_pose;
        }
    }
    int32_t max_sq_dist = tuning->max_sq_dist_expected;
    uint16_t number_correspondance = 0;
    uint8_t indexs_possibles_candidates[MAX_NB_BEACONS][MAX_CANDIDATES_BEACONS];
    memset(indexs_possibles_candidates, 255, sizeof(indexs_possibles_candidates));
    point_t expected_positions[MAX_NB_BEACONS];
    point_t refinement_beacons_positions[MAX_NB_BEACONS];
    point_t actual_positions[MAX_NB_BEACONS];
    memset(last_assos, 0, sizeof(last_assos));

    calculate_coor:
        calc_expected_beacon_pos(estimated_odom, expected_positions);
        for (uint16_t i = 0; i < MAX_NB_BEACONS; i++)
        {
            uint8_t status = indexs_closest_amalg(indexs_possibles_candidates[i], expected_positions[i], 
            candidates, nb_amalgs, max_sq_dist);
            if(status != 1) {
                for (size_t j = 0; j < MAX_CANDIDATES_BEACONS; j++)
                {
                    indexs_debug[j] = indexs_possibles_candidates[i][j];
                }
                
                return too_many_candidates_pose;
                //TODO error management 
            }
        }
        uint8_t correspondance[MAX_NB_BEACONS] = {255, 255, 255};
        //TODO : proper typing on find_correspondance
        uint8_t nb_corr = find_correspondance(correspondance, indexs_possibles_candidates, candidates, nb_amalgs, tuning);
        //If can't find beacon using odometry, do it once using larger area (& last lidar data)
        if(nb_corr == 11111) {
            return too_many_corr_pose;
        }
        if(nb_corr < 2 && max_sq_dist != tuning->max_sq_dist_large_expected) {
            estimated_odom = estimated_lidar;
            max_sq_dist = tuning->max_sq_dist_large_expected;
            goto calculate_coor;
        }
    for (uint8_t i = 0; i < MAX_NB_BEACONS; i++)
    {
        if(correspondance[i] != 255) {
            refinement_beacons_positions[number_correspondance] = beacon_positions[i];
            actual_positions[number_correspondance++] = candidates[correspondance[i]];
            last_assos[i] = correspondance[i]; //For display purposes
        }
    }

        //TODO : 
        //actual_positions[i] = refine_pos_beacon(amalgames[i].pts->angles, amalgames[i].pts->distances);
    

    //ESP_LOGI(TAG, "Refining pose with %d identified beacons - %d rejects", number_of_identified_beacons, number_of_rejects);
    pose_t refined_pose = find_pose_from_beacons(refinement_beacons_positions, actual_positions, 
        (size_t) number_correspondance, estimated_lidar);

    estimated_lidar = refined_pose;
    return refined_pose;
    //ESP_LOGI(TAG, "Pose refined to %f %f %f", refined_pose.x, refined_pose.y, refined_pose.theta);
    //xQueueOverwrite(output_pose_queue, &refined_pose);

}
static void calc_expected_beacon_pos(pose_t estimated_pose, point_t* expected_positions) {
    for (uint16_t i = 0; i < MAX_NB_BEACONS; i++)
    {
        int32_t relative_x = beacon_positions[i].x - estimated_pose.pos.x;
        int32_t relative_y = beacon_positions[i].y - estimated_pose.pos.y;

        float cos_theta = cos(-estimated_pose.angle_rad);
        float sin_theta = sin(-estimated_pose.angle_rad);

        expected_positions[i].x = relative_x * cos_theta - relative_y * sin_theta;
        expected_positions[i].y = relative_x * sin_theta + relative_y * cos_theta;
    }
    
}

//TODO : gérer cas de overlap de points par exemple (255, 12,12, 25)
//returns number of beacons in "best" correspondance
static uint8_t find_correspondance(uint8_t* correspondances, uint8_t indexs[MAX_NB_BEACONS][MAX_CANDIDATES_BEACONS], 
            const point_t candidates[], size_t nb_candidates, pose_tuning_t* tuning) {
    pose_t a,b;
    uint32_t square_dist;

    valid_combination_t combinations[MAX_COMBINATIONS];
    //TODO : manage case 2
    size_t nb_comb = generate_combination(combinations, MAX_COMBINATIONS, indexs, candidates, tuning->max_dist_beacons);
    if (nb_comb >= MAX_COMBINATIONS) {
        //TODO ERROR
    }
    uint8_t counts[MAX_COMBINATIONS];
    size_t max_non_255 = 0;

    for (size_t i = 0; i < nb_comb; i++) {
        uint8_t count = 0;
        for (int j = 0; j < MAX_NB_BEACONS; j++) {
            if (combinations[i].index[j] != 255) {
                count++;
            }
        }
        counts[i] = count;
        if (count >= 2 && count > max_non_255) {
            max_non_255 = count;
        }
    }

    // Process combinations that meet the criteria
    uint8_t nb_max_length = 0;
    uint16_t index_valid_comb = 0;
    for (size_t i = 0; i < nb_comb; i++) {
        if (counts[i] >= 2 && counts[i] == max_non_255) {
            nb_max_length++;
            index_valid_comb = i;
        }
    }
    for (size_t i = 0; i < MAX_NB_BEACONS; i++)
    {
        correspondances[i] = combinations[index_valid_comb].index[i];
    }
    
    if(nb_max_length > 255) { //TODO : change this RANDOM VALUE
        return 11111; //TODO : ERROR TO HANDLE
    }
    return max_non_255;
}

//REturns 0 if reaching max amount  candidates
static uint8_t indexs_closest_amalg(uint8_t indexs[MAX_CANDIDATES_BEACONS], point_t expected, const point_t candidates[], 
    size_t nb_candidates, int32_t max_sq_dist) {
    uint8_t cur_index = 0;
    for (size_t i = 0; i < nb_candidates; i++)
    {
        int32_t dist = SQUARE(expected.x - candidates[i].x) + SQUARE(expected.y - candidates[i].y);
        if(dist < max_sq_dist) {
            indexs[cur_index++] = i;
        }
        if (cur_index >= MAX_CANDIDATES_BEACONS - 1)
        {
            return 0;
        }        
    }
    return 1;
}

//Returns length of number of combination
static size_t generate_combination(valid_combination_t* combinations, uint16_t max_combinations, 
    const uint8_t indexs[MAX_NB_BEACONS][MAX_CANDIDATES_BEACONS], const point_t candidates[], uint32_t max_dist) {
    size_t combination_count = 0;
    uint8_t temp_combinations[MAX_NB_BEACONS][MAX_CANDIDATES_BEACONS];
    uint8_t nb_candidates[MAX_NB_BEACONS];
    memset(temp_combinations, 255, sizeof(temp_combinations));
    memset(nb_candidates, 1, sizeof(nb_candidates));

    //Filter valid beacons with distance of "first beacon" in the combination
    for (size_t beacon = 0; beacon < MAX_NB_BEACONS-1; ++beacon) {
        for (size_t i = 0; i < MAX_CANDIDATES_BEACONS; ++i) {
            if (indexs[beacon][i] == 255) {
                break; // No more valid candidates for this beacon
            }

            //generate combination keeping order (exemple : [1, [3,4], [6,7]])
            for (size_t other_beacon = beacon+1; other_beacon < MAX_NB_BEACONS; other_beacon++)
            {
                size_t temp_idx_cand_other = 0;
                for (size_t j = 0; j < MAX_CANDIDATES_BEACONS; ++j) {
                    if (indexs[other_beacon][j] == 255) {
                        break;
                    }
                    float dist_beacons = sqrt(sq_dist(candidates[indexs[beacon][i]],
                    candidates[indexs[other_beacon][j]]));
                    if (in_range(dist_beacons, distances_beacons[beacon][other_beacon], max_dist)) {
                        temp_combinations[other_beacon][temp_idx_cand_other++] = indexs[other_beacon][j];
                        nb_candidates[other_beacon]++;
                    }
                }
            }
    
           uint16_t possibilities = 1;
            for (size_t j = 0; j < MAX_NB_BEACONS; j++)
            {
                possibilities *= nb_candidates[j];
            }

            if(combination_count + possibilities >= max_combinations) {
                return max_combinations; //ERROR : too many combinations found
            }
            for (uint16_t combo_id = combination_count; combo_id < combination_count + possibilities; combo_id++) {
                // Calculate indices for each array
                uint16_t divisor = possibilities;
                for (int8_t j = 0; j < MAX_NB_BEACONS; j++) {
                    divisor /= nb_candidates[j];
                    uint8_t array_index = (combo_id / divisor) % nb_candidates[j];
                    combinations[combo_id].index[j] = temp_combinations[j][array_index];
                }
                combinations[combo_id].index[beacon] = indexs[beacon][i]; //Force current "first compared beacon"
            }
            combination_count += possibilities;
        }
    }
    return combination_count;



    
}

//update field "nb_beacons" of the valid_combination
//returns status : 1 if no problem, 0 if multiple have same count
static uint8_t update_valid_combination(valid_combination_t* valid_combination, valid_combination_t combination_add) {
    uint8_t count = 0;
    for (size_t i = 0; i < MAX_NB_BEACONS; i++)
    {
        if(combination_add.index[i] != 255){
            count++;
        }
    }
    if (valid_combination->nb_beacons == count) {
        return 0;
    }
    if (valid_combination->nb_beacons < count)
    {
        valid_combination->nb_beacons = 0;
                for (size_t i = 0; i < MAX_NB_BEACONS; i++) {
            valid_combination->index[i] = combination_add.index[i];
        }
        return 1;
    }
    
}

static float sq_dist(point_t a, point_t b) {
    return SQUARE(a.x - b.x) + SQUARE(a.y - b.y); 
}

static bool in_range(float dist, float expected_dist, float err_tolerance) {
    return abs(dist-expected_dist) < err_tolerance;
} 