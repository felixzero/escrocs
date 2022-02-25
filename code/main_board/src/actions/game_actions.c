#include "actions/game_actions.h"

#include "motion/motion_control.h"
#include "peripherals/stepper_board.h"

struct GAME_ACTION_OUTPUT_STRUCT_NAME(set_pose) game_action_set_pose(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(set_pose) args)
{
    pose_t target;
    target.x = args.x;
    target.y = args.y;
    target.theta = args.theta;
    set_motion_target(&target);
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(set_pose) result;
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(get_pose) game_action_get_pose(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(get_pose) args)
{
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(get_pose) result;
    pose_t current_pose = get_current_pose();
    result.x = current_pose.x;
    result.y = current_pose.y;
    result.theta = current_pose.theta;
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(stop_motion) game_action_stop_motion(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(stop_motion) args)
{
    stop_motion();
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(stop_motion) result;
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(set_pump) game_action_set_pump(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(set_pump) args)
{
    set_stepper_board_pump(args.channel, args.value);
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(set_pump) result;
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(move_stepper) game_action_move_stepper(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(move_stepper) args)
{
    move_stepper_board_motor(args.channel, args.target, args.speed, args.acceleration);
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(move_stepper) result;
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(reset_stepper) game_action_reset_stepper(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(reset_stepper) args)
{
    define_stepper_board_motor_home(args.channel, args.value);
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(reset_stepper) result;
    return result;
}
