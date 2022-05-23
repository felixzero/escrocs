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

function deposit_replique()
    move_stepper(1, 4000, 0.02)
    sleep(3.0)
end

-- Initial setup
overwrite_known_pose(242, 825, -pi / 2)

-- Move to piedestal and pick statuette
move_to_position(242, 1054, nil)
move_to_position(465, 1530, -3 * pi / 4)
move_arm_to_pick_statuette()
move_to_position(250, 1750, nil)
move_arm_to_secure_statuette()
move_to_position(465, 1530, -pi / 4)
move_to_position(350, 1650, nil)
deposit_replique()
move_to_position(465, 1530, nil)

-- Go to vitrine
move_to_position(237, 245, pi / 2)
move_to_position(237, 143, nil)
move_to_position(237, 245, nil)

-- Go back home
move_to_position(237, 487, nil)
