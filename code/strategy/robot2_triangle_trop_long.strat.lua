
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
        move_stepper(channel, 2200, 0.2)
    else
        move_stepper(channel, -2200, 0.2)
    end
end

function lower_pump_to_medium_layer(channel)
    if channel == 1 then
        move_stepper(channel, 1550, 0.2)
    else
        move_stepper(channel, -1550, 0.2)
    end
end

function lower_pump_to_top_layer(channel)
    if channel == 1 then
        move_stepper(channel, 1000, 0.2)
    else
        move_stepper(channel, -1000, 0.2)
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
cakebase_pos = 600
safe_rotation_pos = 500

side = get_button(22)

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
sleep(2.0)

-- Slurp the cake 3
move_to_position(cakebase_pos, y_pos_initial, -pi/3)
lower_pump_to_floor(pump_0)
sleep(3.0)
set_pump(pump_0, true)
sleep(2.0)
set_pump_to_safe_height(pump_0)
sleep(1.0)

-- Deposit layer 1 to base
function deposit_to_base()
    base_y = 222
    safe_base_x = 572
    base_x = 165
    move_to_position(safe_base_x, base_y, -pi/3)
    move_to_position(base_x, base_y, -pi/3)
    -- Drop cakes
    set_pump(pump_1, false)
    set_pump(pump_2, false)
    sleep(2.0)
    move_to_position(safe_base_x, base_y, -pi/3)
    move_to_position(safe_base_x, base_y, -pi/3 + pi)
    move_to_position(base_x, base_y, -pi/3 + pi)
    set_pump(pump_0, false)
    sleep(2.0)
end
deposit_to_base()

-- Fetch second layer
function fetch_second_layer(second_layer_y)
    second_layer_x = 128

    move_to_position(safe_base_x, second_layer_y, -pi/3 + pi)
    move_to_position(second_layer_x, second_layer_y, -pi/3 + pi)
    lower_pump_to_floor(pump_0)
    sleep(3.0)
    set_pump(pump_0, true)
    sleep(2.0)
    set_pump_to_safe_height(pump_0)
    sleep(1.0)
    move_to_position(safe_base_x, second_layer_y, -pi/3 + pi)
    move_to_position(safe_base_x, second_layer_y, pi/3 + pi)
    move_to_position(second_layer_x, second_layer_y, pi/3 + pi)
    lower_pump_to_medium_layer(pump_1)
    sleep(3.0)
    set_pump(pump_1, true)
    sleep(2.0)
    set_pump_to_safe_height(pump_1)
    sleep(1.0)
    move_to_position(safe_base_x, second_layer_y, pi/3 + pi)
    move_to_position(safe_base_x, second_layer_y, -pi + pi)
    move_to_position(second_layer_x, second_layer_y, -pi + pi)
    lower_pump_to_top_layer(pump_2)
    sleep(3.0)
    set_pump(pump_2, true)
    sleep(2.0)
    set_pump_to_safe_height(pump_2)
    sleep(1.0)
    move_to_position(safe_base_x, second_layer_y, -pi + pi)
end
fetch_second_layer(557)
deposit_to_base()
fetch_second_layer(366)
deposit_to_base()

-- Return to base
move_to_position(cakebase_pos, y_pos_initial, -pi)
move_to_position(x_pos_initial, y_pos_initial, -pi)
