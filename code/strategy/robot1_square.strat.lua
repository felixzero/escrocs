map_size_y = 2000
pi = 3.1415926535

function overwrite_known_pose(x, y, theta)
    overwrite_pose(x, map_size_y - y, theta)
end

function move_to_position(x, y, theta)
    set_pose(x, map_size_y - y, theta, false)
    sleep(0.5)
    while not is_motion_done() do
        sleep(0.5)
    end
end

function move_to_position_with_detection(x, y, theta)
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

function move_arm_to_pick_statuette()
    move_stepper(0, 1400, 0.15)
    sleep(3)
end

function move_arm_to_secure_statuette()
    move_stepper(0, 1200, 0.15)
    sleep(0.5)
    move_stepper(0, 500, 0.15)
    sleep(3.0)
end

function move_arm_to_deposit_statuette()
    move_stepper(0, 800, 0.15)
    sleep(3.0)
end

function deposit_replique()
    --move_stepper(1, -2000, 0.02)
    sleep(6.0)
end

function move_straight(distance, timeout)
    x, y, theta = get_pose()
    unsafe_move_to_position(x + distance * math.cos(theta), map_size_y -y - distance * math.sin(theta), nil, timeout)
end

-- Initial setup and leaving zone
x_initial = 190
y_initial = 882
overwrite_known_pose(x_initial, y_initial, 0)
side = get_button(22)
sleep(30)

-- Move to piedestal and pick statuette
x_safe_pied = 510
y_safe_pied = 1500
x_contact_pied = 300
y_contact_pied = 1700
move_to_position(x_safe_pied, y_initial, nil)
move_to_position(x_safe_pied, y_safe_pied, -3 * pi / 4)
move_arm_to_pick_statuette()
--unsafe_move_to_position(x_contact_pied, y_contact_pied, nil, 4)
move_straight(400, 4)
move_arm_to_secure_statuette()
move_to_position(x_safe_pied, y_safe_pied, pi / 4)
--move_straight(-400, 4)
--set_pose(nil, nil, pi / 4, true)
sleep(2)
--unsafe_move_to_position(x_contact_pied, y_contact_pied, nil, 4)
--move_straight(-400, 4)
--deposit_replique()
--move_to_position(x_safe_pied, y_safe_pied, nil)

move_to_position_with_detection(300, 400, pi / 2)

set_pose(nil, nil, -pi, false)
sleep(0.5)
while not is_motion_done() do
    sleep(0.5)
end
move_straight(350, 4)
move_straight(-80, 2)
set_pose(nil, nil, pi / 2, false)
sleep(0.5)
while not is_motion_done() do
    sleep(0.5)
end
move_stepper(0, 900, 0.15)
sleep(2.0)
move_straight(350, 3)
if not side then
    move_straight(-30, 2)
end
move_stepper(0, 1600, 0.15)
sleep(2.0)
move_straight(-150, 4)
move_stepper(0, 2200, 0.15)
sleep(2.0)
move_straight(150, 4)

move_straight(-400, 4)


--[[
-- Go to vitrine
x_vitrine = 200
y_safe_vitrine = 245
y_contact_vitrine = 100
move_to_position(x_vitrine, y_safe_vitrine, pi / 2)
unsafe_move_to_position(x_vitrine, y_contact_vitrine, nil, 5)
move_arm_to_deposit_statuette()
sleep(3.0)
move_to_position(x_vitrine, y_safe_vitrine, nil)

-- Go back home
move_to_position(x_vitrine, 487, nil)
--]]