x_initial, y_initial, theta_initial = 0, 0, 0

function on_init(side)
    move(100, 0, 0)
    overwrite_pose(100, y_initial, theta_initial)
    move_servo(0, 8500)
end

function move(x, y, t)
	set_pose(x, y, t, false)
    sleep(0.2)
	while not is_motion_done() do
		sleep(0.2)
	end
end

function on_run()
    move(-100, 0, 0)
    overwrite_pose(x_initial, y_initial, theta_initial)
    move_servo(0, 6000)
    sleep(0.2)
    move_servo(0, 4000)
    sleep(0.2)
    move_servo(0, 2000)
    sleep(1.0)
    move(100, 0, 0)
    move(1800, -200, 0)
end

function resume_loop()
    return 100
end
