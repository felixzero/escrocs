
map_size_y = 3000
map_size_x = 2000
pi = 3.1415926535

function overwrite_known_pose(x, y, theta)
    overwrite_pose(x, map_size_y - y, theta)
end

function move_to_position(x, y, theta)
    set_pose(x, map_size_y - y, theta, true)
    sleep(0.5)
    while not is_motion_done() do
        sleep(0.5)
    end
end

function move_to_position_no_detect(x, y, theta)
    set_pose(x, map_size_y - y, theta, false)
    sleep(0.5)
    while not is_motion_done() do
        sleep(0.5)
    end
end

function unsafe_move_to_position(x, y, theta, timeout)
    set_pose(x, map_size_y - y, theta, false)
    sleep(0.5)
    for i = timeout, 1, -1 do
        if is_motion_done() then
            return
        end
        sleep(1.0)
    end
    stop_motion()
end

function move_to_position_or_give_up(x, y, theta, timeout)
    set_pose(x, map_size_y - y, theta, true)
    sleep(0.5)
    for i = timeout, 1, -1 do
        if is_motion_done() then
            return true
        end
        sleep(1.0)
    end
    stop_motion()
    return false
end

function lower_pump_to_floor(channel)
    if channel == 1 then
        move_stepper(channel, 2100, 0.2)
    else
        move_stepper(channel, -2100, 0.2)
    end
end

function lower_pump_to_medium_layer(channel)
    if channel == 1 then
        move_stepper(channel, 1450, 0.2)
    else
        move_stepper(channel, -1450, 0.2)
    end
end

function lower_pump_to_top_layer(channel)
    if channel == 1 then
        move_stepper(channel, 900, 0.2)
    else
        move_stepper(channel, -900, 0.2)
    end
end

function set_pump_to_safe_height(channel)
    if channel == 1 then
        move_stepper(channel, 500, 0.2)
    else
        move_stepper(channel, -500, 0.2)
    end
end

function raise_pump_to_up_position(channel)
    move_stepper(channel, 0, 0.15)
end

function raise_arm(channel)
    move_servo(channel, 3000)
end

function lower_arm(channel)
    move_servo(channel, 6500)
end




-- Initial setup
side = get_button(22)
pump_0 = 0
pump_1 = 1
pump_2 = 2
if side then
    pump_0 = 1
    pump_1 = 0
end
pumps = { pump_0, pump_1, pump_2 }

x_pos_initial = 140
y_pos_initial = 1880
cakebase_pos = 570
safe_rotation_pos = 500

overwrite_known_pose(x_pos_initial, y_pos_initial, -pi)



-- Move to cake base 1
move_to_position(cakebase_pos, y_pos_initial, -pi)

-- Slurp the cake 1
lower_pump_to_top_layer(pump_2)
sleep(3.0)
set_pump(pump_2, true)
sleep(2.0)
set_pump_to_safe_height(pump_2)
sleep(1.0)

-- Rotate the robot
move_to_position(safe_rotation_pos, y_pos_initial, -pi)
move_to_position(safe_rotation_pos, y_pos_initial, pi/3)

-- Slurp the cake 2
move_to_position(cakebase_pos, y_pos_initial, pi/3)
lower_pump_to_medium_layer(pump_1)
sleep(3.0)
set_pump(pump_1, true)
sleep(2.0)
set_pump_to_safe_height(pump_1)
sleep(1.0)

-- Rotate the robot
move_to_position(safe_rotation_pos, y_pos_initial, pi/3)
move_to_position(safe_rotation_pos, y_pos_initial, -pi/3)

-- Slurp the cake 3
move_to_position(cakebase_pos, y_pos_initial, -pi/3)
lower_pump_to_floor(pump_0)
sleep(3.0)
set_pump(pump_0, true)
sleep(2.0)
set_pump_to_safe_height(pump_0)
sleep(1.0)

move_to_position(safe_rotation_pos, y_pos_initial, -pi/3)
move_to_position(safe_rotation_pos, y_pos_initial, -pi/3 - pi)

-- Deposit in plate
x_deposition = 206
move_to_position(safe_rotation_pos, y_pos_initial, -pi/3 - pi)
move_to_position_no_detect(x_deposition, y_pos_initial, -pi/3 - pi)
set_pump(pump_0, false)
move_to_position(safe_rotation_pos, y_pos_initial, -pi/3 - pi)
move_to_position(safe_rotation_pos, y_pos_initial, pi/3 - pi)
move_to_position_no_detect(x_deposition, y_pos_initial, pi/3 - pi)
set_pump(pump_1, false)
move_to_position(safe_rotation_pos, y_pos_initial, pi/3 - pi)
move_to_position(safe_rotation_pos, y_pos_initial, -pi - pi)
move_to_position_no_detect(x_deposition, y_pos_initial, -pi - pi)
set_pump(pump_2, false)
move_to_position(safe_rotation_pos, y_pos_initial, -pi - pi)

-- Go to home
move_to_position(840, y_pos_initial, -pi - pi)
move_to_position(840, 630, -pi -pi)
home_x = 530
home_y = 180
raise_arm(2)
move_to_position_no_detect(home_x, home_y, -pi -pi)
