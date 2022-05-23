
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
        move_stepper(channel, 2200, 0.2)
    else
        move_stepper(channel, -2200, 0.2)
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
overwrite_known_pose(258, 544, 0)
lower_arm(0)
lower_arm(1)
lower_arm(2)

-- First, fetch samples in central area
move_to_position(680, 677, -pi / 3)
set_pump(0, true)
lower_pump_to_floor(0)
sleep(4.0)
raise_pump_to_up_position(0)
sleep(1.5)
move_to_position(480, 677, -pi / 3)
move_to_position(480, 677, 2 * pi / 3)
move_to_position(823, 669, 2 * pi / 3)
set_pump(1, true)
set_pump(2, true)
lower_pump_to_floor(1)
lower_pump_to_floor(2)
sleep(4.0)
raise_pump_to_up_position(1)
raise_pump_to_up_position(2)
sleep(1.5)

-- Deposit samples in gallery
x_depositions = { 572, 808, 1046 }
theta_depositions = { pi / 6, pi / 6 + 2 * pi / 3, pi / 6 + 4 * pi / 3 }
y_deposition_safe = 275
y_deposition_contact = 173

for i = 1, 3, 1 do
    move_to_position(x_depositions[i], y_deposition_safe, theta_depositions[i])
    raise_arm(i - 1)
    unsafe_move_to_position(x_depositions[i], y_deposition_contact, theta_depositions[i], 3)
    set_pump(i - 1, false)
    sleep(1.5)
    move_to_position(x_depositions[i], y_deposition_safe, theta_depositions[i])
    lower_arm(i - 1)
end

-- Flip carre de fouille
x_flips = { 1777, 1592, 1407, 1222, 852 }
y_flip_safe = 1690
y_flip_contact = 1910
move_to_position(x_flips[1], y_flip_safe, -pi / 6)
lower_pump_to_floor(1)
sleep(3)
for i = 1, #x_flips do
    move_to_position(x_flips[i], y_flip_safe, -pi / 6)
    raise_arm(1)
    unsafe_move_to_position(x_flips[i], y_flip_contact, -pi / 6, 2)
    move_to_position(x_flips[i], y_flip_safe, -pi / 6)
    lower_arm(1)
end
lower_arm(1)

-- Finally go home
move_to_position(390, 1000, 0)

