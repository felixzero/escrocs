
map_size_y = 2000
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
        move_stepper(channel, 2400, 0.2)
    else
        move_stepper(channel, -2400, 0.2)
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
overwrite_known_pose(160, 560, 0)
side = get_button(22)
pump_0 = 0
pump_1 = 1
pump_2 = 2
if side then
    pump_0 = 1
    pump_1 = 0
end
pumps = { pump_0, pump_1, pump_2 }

lower_arm(pump_0)
lower_arm(pump_1)
lower_arm(pump_2)

-- Safely move away from other robot
unsafe_move_to_position(0, 560, 0, 2)
sleep(1.0)
overwrite_known_pose(30, 560, 0)

move_to_position(300, 300, 0)

-- First, fetch samples in central area
pick_x = 700
safe_pick_x = 580
pick_y = 730
two_pick_x = 860
two_pick_y = 729
move_to_position(pick_x, pick_y, -pi / 3)
set_pump(pump_0, true)
lower_pump_to_floor(pump_0)
sleep(4.0)
raise_pump_to_up_position(pump_0)
sleep(1.5)
unsafe_move_to_position(safe_pick_x, pick_y, -pi / 3, 20)
unsafe_move_to_position(safe_pick_x, pick_y, 2 * pi / 3, 20)
move_to_position(two_pick_x, two_pick_y, 2 * pi / 3)
set_pump(pump_1, true)
set_pump(pump_2, true)
lower_pump_to_floor(pump_1)
lower_pump_to_floor(pump_2)
sleep(4.0)
raise_pump_to_up_position(pump_1)
raise_pump_to_up_position(pump_2)
sleep(1.5)

-- Deposit samples in gallery
x_depositions = { 572, 808, 1046 }
theta_depositions = { pi / 6, pi / 6 + 2 * pi / 3, pi / 6 + 4 * pi / 3 }
y_deposition_safe = 275
y_deposition_contact = 170

for i = 1, 3, 1 do
    raise_arm(pumps[i])
    move_to_position(x_depositions[i], y_deposition_safe, theta_depositions[i])
    unsafe_move_to_position(x_depositions[i], y_deposition_contact, theta_depositions[i], 3)
    set_pump(pumps[i], false)
    sleep(1.5)
    move_to_position(x_depositions[i], y_deposition_safe, theta_depositions[i])
    lower_arm(pumps[i])
end

move_to_position(x_depositions[2], y_deposition_safe, theta_depositions[3])
unsafe_move_to_position(x_depositions[2], 0, theta_depositions[3], 4)
overwrite_known_pose(x_depositions[2], 180, theta_depositions[3])

-- Flip carre de fouille
x_flips = { 1777 + 100, 1592 + 100, 1407 + 100, 1222 + 100 }
y_flip_safe = 1720
y_flip_contact = 2200
move_to_position(x_flips[1], y_flip_safe, -pi / 6)
lower_pump_to_floor(pump_1)
sleep(3)
for i = 1, #x_flips do
    move_to_position(x_flips[i], y_flip_safe, -pi / 6)
    raise_arm(pump_1)
    unsafe_move_to_position(x_flips[i], y_flip_contact, -pi / 6, 3)
    overwrite_known_pose(x_flips[i], 1990, -pi / 6, 2)
    move_to_position(x_flips[i], y_flip_safe, -pi / 6)
    lower_arm(pump_1)
end
lower_arm(pump_1)
move_to_position(407, 1482, -pi / 6)

-- Finally go home
move_to_position(290, 1000, 0)

