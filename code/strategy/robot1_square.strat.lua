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
    move_stepper(1, -2000, 0.02)
    sleep(6.0)
end

-- Initial setup and leaving zone
overwrite_known_pose(300, 830, -pi / 2)
move_to_position(300, 1180, nil)

-- Move to piedestal and pick statuette
x_safe_pied = 510
y_safe_pied = 1490
x_contact_pied = 300
y_contact_pied = 1700
move_to_position(x_safe_pied, y_safe_pied, -3 * pi / 4)
move_arm_to_pick_statuette()
unsafe_move_to_position(x_contact_pied, y_contact_pied, nil, 4)
move_arm_to_secure_statuette()
move_to_position(x_safe_pied, y_safe_pied, pi / 4)
unsafe_move_to_position(x_contact_pied, y_contact_pied, nil, 4)
deposit_replique()
move_to_position(x_safe_pied, y_safe_pied, nil)

-- Go to vitrine
x_vitrine = 190
y_safe_vitrine = 245
y_contact_vitrine = 100
move_to_position(x_vitrine, y_safe_vitrine, pi / 2)
move_arm_to_deposit_statuette()
unsafe_move_to_position(x_vitrine, y_contact_vitrine, nil, 5)
sleep(3.0)
move_to_position(x_vitrine, y_safe_vitrine, nil)

-- Go back home
move_to_position(x_vitrine, 487, nil)
