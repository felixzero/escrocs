#include "pose_refinement.h"

#include "amalgame.h"
#include "math.h"

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

uint32_t distances_beacons[MAX_NB_BEACONS][MAX_NB_BEACONS] = {
    0, 0, 0
};//COmment faire distances beacons ??

pose_t refined_lidar;
uint16_t last_assos[MAX_NB_BEACONS];
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
    uint16_t number_correspondance = 0;
    uint8_t indexs_possibles_candidates[MAX_NB_BEACONS][MAX_CANDIDATES_BEACONS];
    point_t expected_positions[MAX_NB_BEACONS];
    point_t refinement_beacons_positions[MAX_NB_BEACONS];
    point_t actual_positions[MAX_NB_BEACONS];

    calc_expected_beacon_pos(estimated_odom, expected_positions);
    for (uint16_t i = 0; i < MAX_NB_BEACONS; i++)
    {
        uint8_t status = indexs_closest_amalg(indexs_possibles_candidates[i], expected_positions[i], 
        candidates, nb_amalgs, tuning->max_sq_dist_beacons);
        if(status != 1) {
            //TODO error management 
        }
    }
    uint8_t correspondance[MAX_NB_BEACONS] = {255};
    //TODO : proper typing on find_correspondance
    uint8_t nb_corr = find_correspondance(correspondance, indexs_possibles_candidates, candidates, nb_amalgs, tuning);

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

//returns number of beacons in "best" correspondance
static uint8_t find_correspondance(uint8_t* correspondances, const indexs[MAX_NB_BEACONS][MAX_CANDIDATES_BEACONS], 
            const point_t candidates[], size_t nb_candidates, pose_tuning_t* tuning) {
    pose_t a,b;
    uint32_t square_dist;
    valid_combination_t valid_combination = {
        .nb_beacons = 0,
    };
    many_corr = 1; 

    for (uint8_t beacon_i = 0; beacon_i < MAX_NB_BEACONS; beacon_i++)
    {
        //foreach amalgame that could be a beacon, check every "geometric shape" : 
        for (uint8_t beacon_candidate_i = 0; beacon_candidate_i < MAX_CANDIDATES_BEACONS; beacon_candidate_i++)
        {
            uint8_t matchs[MAX_NB_BEACONS][MAX_CANDIDATES_BEACONS] = {255};
            point_t a = candidates[indexs[beacon_i][beacon_candidate_i]];

            // Remplissage de matchs :
            for (uint8_t other_i = beacon_i+1; other_i < MAX_NB_BEACONS; other_i++)
            {
                uint8_t index_of_match = 0;
                for (uint8_t other_candidate_i = 0; other_candidate_i < MAX_CANDIDATES_BEACONS; other_candidate_i++)
                {
                    point_t b = candidates[indexs[other_i][other_candidate_i]];
                    square_dist = SQUARE(a.x - b.x) + SQUARE(a.y - b.y);
                    if(fabs(square_dist - distances_beacons[beacon_i][other_i]) < tuning->max_sq_dist_beacons) {
                        matchs[beacon_candidate_i][index_of_match++] = b;
                    }

                }
                
            }

            valid_combination_t* combinations;
            uint8_t nb_comb = generate_combination(combinations, matchs);
            uint8_t unvalid_combination = 0;

            //Verifications de combinaisons, on ne garde que les combinaisons valide les plus longues : 
            for (size_t comb_i = 0; comb_i < nb_comb; comb_i++)
            {
                for (size_t cur_beacon = 1; cur_beacon < MAX_NB_BEACONS; cur_beacon++)
                {
                    if(combinations[comb_i].index[cur_beacon-1] == 255 || combinations[comb_i].index[cur_beacon] == 255) {
                        continue;
                    }
                    point_t a = candidates[cur_beacon-1][combinations[comb_i].index[cur_beacon-1]];
                    point_t b = candidates[cur_beacon][combinations[comb_i].index[cur_beacon]];
                    uint32_t square_dist = SQUARE(a.x - b.x) + SQUARE(a.y - b.y);
                    if(fabs(square_dist - distances_beacons[cur_beacon-1][cur_beacon]) > tuning.max_sq_dist_beacons) {
                        unvalid_combination = 1;
                        break;
                    }
                }
                if (unvalid_combination)
                {
                    unvalid_combination = 0;
                    continue;
                }
                many_corr = update_valid_combination(valid_combination, combinations[comb_i]);
                
            }
        }
    }
    for (size_t i = 0; i < MAX_NB_BEACONS; i++)
    {
        correspondances[i] = valid_combination.index[i];
    }
    return valid_combination.nb_beacons;
}

static uint8_t calculate_correspondance()
//REturns 0 if reaching max amount  candidates
static uint8_t indexs_closest_amalg(const uint8_t[MAX_NB_BEACONS][MAX_CANDIDATES_BEACONS] indexs, point_t expected, const point_t candidates[], 
    size_t nb_candidates, pose_tuning_t tuning) {
    uint8_t cur_index = 0;
    for (size_t i = 0; i < nb_candidates; i++)
    {
        int32_t dist = SQUARE(expected.x - candidates[i].x) + SQUARE(expected.y - candidates[i].y);
        if(dist < tuning.max_sq_dist_expected) {
            indexs[cur_index++] = i;
        }
        if (cur_index >= MAX_CANDIDATES_BEACONS)
        {
            return 0;
        }        
    }
    return 1;
}


//generate combination keeping order (exemple : [1, [3,4], [6,7]] -> [1,3,6], [1,4,6], [1,3,7], [1,4,7] )
static size_t generate_combination(valid_combination_t* combinations, const uint8_t[MAX_NB_BEACONS][MAX_CANDIDATES_BEACONS] matchs) {
    uint8_t possibilities[MAX_NB_BEACONS] = {1};
    uint16_t count = 1;

    //calculate number of combinations
    for (size_t i = 0; i < MAX_NB_BEACONS; i++)
    {
        for (size_t amalg = 0; amalg < MAX_CANDIDATES_BEACONS; amalg++)
        {
            if(matchs[i][amalg] == 255) {
                possibilities[i] = amalg+1;
                break;
            }

        }
        count *= possibilities[i];
    }
    
    valid_combination_t* combinations = (valid_combination_t*) malloc(sizeof(valid_combination_t) * count);
    uint8_t max_comb = 0;
    for (size_t beacon = 0; beacon < count; beacon++)
    {
        for (size_t i = 0; i < max_comb; i++)
        {
            combinations[i].index[beacon] = matchs[beacon][0];
        }
        
       for (size_t amalg = 1; amalg < possibilities[beacon]; amalg++)
       {
            for (size_t i = 0; i < max_comb; i++)
            {
                combinations[max_comb] = combinations[i];
                combinations[max_comb].index[beacon] = matchs[beacon][amalg];
            }
            max_comb *= possibilities[beacon]; 
       }
    }
    return count;
    
}

//update field "nb_beacons" of the valid_combination
//returns status : 1 if no problem, 0 if multiple have same count
static uint8_t update_valid_combination(valid_combination_t* valid_combination, valid_combination_t* combination_add) {
    uint8_t count = 0;
    for (size_t i = 0; i < MAX_NB_BEACONS; i++)
    {
        if(combination_add->index[i] != 255){
            count++;
        }
    }
    if (valid_combination.nb_beacons == count) {
        return 0;
    }
    if (valid_combination.nb_beacons < count)
    {
        valid_combination->nb_beacons = 0;
        valid_combination->index = combination_add->index;
        return 1;
    }
    
}