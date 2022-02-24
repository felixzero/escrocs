#include "actions/game_actions.h"

#include "motion/motion_control.h"
#include "peripherals/stepper_board.h"

int game_action_set_pose(int id, struct game_action_pose_data_t data)
{
    pose_t target;
    target.x = data.x;
    target.y = data.y;
    target.theta = data.theta;
    set_motion_target(&target);
    return 0;
}

int game_action_get_pose(int id, struct game_action_pose_data_t *data)
{
    pose_t current_pose = get_current_pose();
    data->x = current_pose.x;
    data->y = current_pose.y;
    data->theta = current_pose.theta;
    return 0;
}

int game_action_set_pump(int id, struct game_action_pump_data_t data)
{
    set_stepper_board_pump(id, data.value);
    return 0;
}

int game_action_get_pump(int id, struct game_action_pump_data_t *data)
{
    return 0;
}

int game_action_set_stepper(int id, struct game_action_stepper_data_t data)
{
    move_stepper_board_motor(id, data.position, data.speed, data.acceleration);
    return 0;
}

int game_action_get_stepper(int id, struct game_action_stepper_data_t *data)
{
    return 0;
}
