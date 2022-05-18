
map_size_y = 2000
pi = 3.1415926535

function overwrite_known_pose(x, y, theta)
    overwrite_pose(x, map_size_y - y, theta)
end

function move_to_position(x, y, theta)
    set_pose(x, map_size_y - y, theta, false)
    while not is_motion_done() do
        sleep(0.5)
    end
    sleep(0.5)
end

function lower_pump_to_floor(channel)
    if channel == 1 then
        move_stepper(channel, 2000, 0.15)
    else
        move_stepper(channel, -2000, 0.15)
    end
    sleep(3.0)
end

function raise_pump_to_up_position(channel)
    move_stepper(channel, 0, 0.15)
    sleep(3.0)
end

function raise_arm(channel)
    sleep(0.5)
    -- Not implemented in FW
end

function lower_arm(channel)
    sleep(0.5)
    -- Not implemented in FW
end

overwrite_known_pose(258, 544, 0)

-- First, fetch samples in central area
move_to_position(680, 677, -pi / 3)
set_pump(0, true)
lower_pump_to_floor(0)
raise_pump_to_up_position(0)
move_to_position(582, 463, 2 * pi / 3)
set_pump(1, true)
set_pump(2, true)
lower_pump_to_floor(1)
lower_pump_to_floor(2)
raise_pump_to_up_position(1)
raise_pump_to_up_position(2)

-- Deposit samples in gallery
x_deposition_0 = 572
x_deposition_1 = 808
x_deposition_2 = 1046
y_deposition_safe = 265
y_deposition_contact = 212
move_to_position(x_deposition_0, y_deposition_safe, pi / 6)
raise_arm(0)
move_to_position(x_deposition_0, y_deposition_contact, pi / 6)
set_pump(0, false)
move_to_position(x_deposition_0, y_deposition_safe, pi / 6)
lower_arm(0)

move_to_position(x_deposition_1, y_deposition_safe, pi / 6)
raise_arm(1)
move_to_position(x_deposition_1, y_deposition_contact, pi / 6)
set_pump(1, false)
move_to_position(x_deposition_1, y_deposition_safe, pi / 6)
lower_arm(1)

move_to_position(x_deposition_2, y_deposition_safe, pi / 6)
raise_arm(2)
move_to_position(x_deposition_2, y_deposition_contact, pi / 6)
set_pump(2, false)
move_to_position(x_deposition_2, y_deposition_safe, pi / 6)
lower_arm(2)

-- Flip carre de fouille
x_flips = { 1777, 1592, 1407, 1222, 852 }
y_flip_safe = 1690
y_flip_contact = 1910
move_to_position(x_flips[1], y_flip_safe, -pi / 6)
raise_arm(1)
for i = 1, #x_flips do
    move_to_position(x_flips[i], y_flip_safe, -pi / 6)
    move_to_position(x_flips[i], y_flip_contact, -pi / 6)
    move_to_position(x_flips[i], y_flip_safe, -pi / 6)
end
lower_arm(1)

-- Fetch samples in distributor
x_fetch_safe = 434
x_fetch_contact = 57
y_fetch = 1250
move_to_position(x_fetch_safe, y_fetch, 0)

fetch_angles = { 0, - 2 * pi / 3, 2 * pi / 3 }
for i = 2, 0, -1 do
    raise_arm(i)
    set_pump(i, true)
    lower_pump_to_floor(i)
    move_to_position(x_fetch_contact, y_fetch, fetch_angles[3 - i])
    raise_pump_to_up_position(i)
    move_to_position(x_fetch_safe, y_fetch, fetch_angles[3 - i])
    lower_arm(i)
end

-- Then deposit under abris
near_abris_x = 413
near_abris_y = 1475
far_abris_x = 529
far_abris_y = 1590
move_to_position(near_abris_x, near_abris_y, pi / 4)
lower_pump_to_floor(2)
set_pump(2, false)
raise_pump_to_up_position(2)
move_to_position(far_abris_x, far_abris_y, pi / 4 - 2 * pi / 3)
lower_pump_to_floor(1)
move_to_position(near_abris_x, near_abris_y, pi / 4 - 2 * pi / 3)
set_pump(1, false)
move_to_position(far_abris_x, far_abris_y, pi / 4 - 4 * pi / 3)
lower_pump_to_floor(0)
move_to_position(near_abris_x, near_abris_y, pi / 4 - 4 * pi / 3)
set_pump(0, false)

-- Finally go home
move_to_position(390, 1000, 0)
