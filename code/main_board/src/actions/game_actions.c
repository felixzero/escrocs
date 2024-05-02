#include "actions/game_actions.h"
#include "system/task_priority.h"
#include "controllers/motion_control.h"
#include "controllers/us_control.h"
#include "peripherals/stepper_board.h"
#include "peripherals/display.h"
#include "peripherals/peripherals.h"
#include "peripherals/motor_board_v3.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>
#include <string.h>

#define TAG             "lua_print"
#define CHANNEL_OFFSET  100

struct GAME_ACTION_OUTPUT_STRUCT_NAME(set_pose) game_action_set_pose(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(set_pose) args)
{
    pose_t target;
    target.x = args.x;
    target.y = args.y;
    target.theta = args.theta;
    set_motion_target(&target, args.perform_detection);
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

struct GAME_ACTION_OUTPUT_STRUCT_NAME(overwrite_pose) game_action_overwrite_pose(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(overwrite_pose) args)
{
    pose_t target;
    target.x = args.x;
    target.y = args.y;
    target.theta = args.theta;
    overwrite_current_pose(&target);
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(overwrite_pose) result;
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(stop_motion) game_action_stop_motion(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(stop_motion) args)
{
    stop_motion();
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(stop_motion) result;
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(is_motion_done) game_action_is_motion_done(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(is_motion_done) args)
{
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(is_motion_done) result;
    result.motion_done = is_motion_done();
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(is_blocked) game_action_is_blocked(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(is_blocked) args)
{
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(is_blocked) result;
    result.motion_done = is_blocked();
    return result;
}

#define PERIPHERAL_CHANNEL_OFFSET 10
struct GAME_ACTION_OUTPUT_STRUCT_NAME(set_pump) game_action_set_pump(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(set_pump) args)
{
    if (args.channel < PERIPHERAL_CHANNEL_OFFSET) {
        set_stepper_board_pump(args.channel, args.value);
    } else {
        set_peripherals_pump(args.channel - PERIPHERAL_CHANNEL_OFFSET, args.value);
    }
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(set_pump) result;
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(move_stepper) game_action_move_stepper(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(move_stepper) args)
{
    move_stepper_board_motor(args.channel, args.target, args.speed);
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(move_stepper) result;
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(move_servo) game_action_move_servo(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(move_servo) args)
{
    set_peripherals_servo_channel(args.channel, args.value);
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(move_servo) result;
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(reset_stepper) game_action_reset_stepper(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(reset_stepper) args)
{
    define_stepper_board_motor_home(args.channel, args.value);
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(reset_stepper) result;
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(get_button) game_action_get_button(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(get_button) args)
{
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(get_button) result;
    if (args.channel < CHANNEL_OFFSET) {
        result.status = read_switch(args.channel);
    } else {
        result.status = read_peripherals_motor_input(args.channel / CHANNEL_OFFSET - 1, args.channel % CHANNEL_OFFSET);
    }
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(sleep) game_action_sleep(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(sleep) args)
{
    vTaskDelay(pdMS_TO_TICKS((int)(args.delay * 1000)));
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(sleep) result;
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(print) game_action_print(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(print) args)
{
    static char last_lua_print[32] = "";

    ESP_LOGI(TAG, "%s", args.message);
    if (strncmp(last_lua_print, args.message, 32) != 0) {
        lcd_printf(1, args.message);
        strncpy(last_lua_print, args.message, 32);
    }
    
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(print) result;
    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(get_us_readings) game_action_get_us_readings(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(get_us_readings) args)
{
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(get_us_readings) result;
    result.us1 = distances[0];
    result.us2 = distances[1];
    result.us3 = distances[2];
    result.us4 = distances[3];
    result.us5 = distances[4];
    result.us6 = distances[5];
    result.us7 = distances[6];
    result.us8 = distances[7];
    result.us9 = distances[8];
    result.us10 = distances[9];
    ESP_LOGI("game_actions[H]", "Distances: %d %d %d %d %d %d %d %d %d %d", distances[0], distances[1], distances[2], distances[3], distances[4], distances[5], distances[6], distances[7], distances[8], distances[9]);

    return result;
}

struct GAME_ACTION_OUTPUT_STRUCT_NAME(scan_channels) game_action_scan_channels(struct GAME_ACTION_ARGUMENTS_STRUCT_NAME(scan_channels) args)
{
    bool channels[10] = {false};
    for (int i = 0; i < 10; ++i) {
        channels[i] = args.channel_mask & (1 << i);
    }
    ESP_LOGI("game_actions[H]", "Scan channels: %d %d %d %d %d %d %d %d %d %d", channels[0], channels[1], channels[2], channels[3], channels[4], channels[5], channels[6], channels[7], channels[8], channels[9]);
    xQueueSend(strategy_single_channel_queue, &channels, 0);
    struct GAME_ACTION_OUTPUT_STRUCT_NAME(scan_channels) result;
    return result;
}
