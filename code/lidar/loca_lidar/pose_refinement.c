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
pose_t refined_lidar;
uint16_t last_assos[MAX_NB_BEACONS];

void convert_xy(point_t* pts, uint16_t count, const uint16_t angles[], const uint16_t distances[]) {
    for (uint16_t i = 0; i < count; i++)
    {
        pts[i].x = (int32_t) (distances[i] * cos(CENTIDEGREES_TO_RADIANS(angles[i])));
        pts[i].y = (int32_t) (distances[i] * sin(CENTIDEGREES_TO_RADIANS(angles[i])));
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
pose_t refine_pose(point_t* candidates, amalgame_t* amalgames, uint16_t nb_candidates, const pose_tuning_t* tuning) {
    if (estimated_lidar.pos.x == 0 && estimated_lidar.pos.y == 0) {
        return empty_pose;
    }
    uint16_t number_rejects = 0;
    uint16_t number_correspondance = 0;
    point_t expected_positions[MAX_NB_BEACONS];
    point_t actual_positions[MAX_NB_BEACONS];

    calc_expected_beacon_pos(expected_positions);
    for (uint16_t i = 0; i < MAX_NB_BEACONS; i++)
    {
        uint8_t index = index_closest_amalg(expected_positions[i], candidates, 
            nb_candidates, tuning->max_sq_dist_expected);
        index==-1 ? number_rejects++ : number_correspondance++;
        actual_positions[i] = candidates[index];
        last_assos[i] = index; //For display purposes
        //TODO : 
        //actual_positions[i] = refine_pos_beacon(amalgames[i].pts->angles, amalgames[i].pts->distances);
    }
    

    //ESP_LOGI(TAG, "Refining pose with %d identified beacons - %d rejects", number_of_identified_beacons, number_of_rejects);
    pose_t refined_pose = find_pose_from_beacons(beacon_positions, actual_positions, 
        (size_t) number_correspondance, estimated_lidar);

    estimated_lidar = refined_pose;
    return refined_pose;
    //ESP_LOGI(TAG, "Pose refined to %f %f %f", refined_pose.x, refined_pose.y, refined_pose.theta);
    //xQueueOverwrite(output_pose_queue, &refined_pose);

}
static void calc_expected_beacon_pos(point_t* expected_positions) {
    for (uint16_t i = 0; i < MAX_NB_BEACONS; i++)
    {
        int32_t relative_x = estimated_lidar.pos.x - beacon_positions[i].x;
        int32_t relative_y = estimated_lidar.pos.y - beacon_positions[i].y;

        float cos_theta = cos(-estimated_lidar.angle_rad);
        float sin_theta = sin(-estimated_lidar.angle_rad);

        expected_positions[i].x = relative_x * cos_theta - relative_y * sin_theta;
        expected_positions[i].y = relative_x * sin_theta + relative_y * cos_theta;
    }
    
}

static int8_t index_closest_amalg(point_t expected, const point_t candidates[], size_t nb_candidates, int32_t max_sq_dist) {
    int32_t min_sq_dist = max_sq_dist;
    int8_t index = -1;
    for (size_t i = 0; i < nb_candidates; i++)
    {
        int32_t dist = SQUARE(expected.x - candidates[i].x) + SQUARE(expected.y - candidates[i].y);
        if(dist < min_sq_dist) {
            min_sq_dist = dist;
            index = i;
        }
    }
    return index;
}