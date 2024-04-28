x_initial, y_initial, theta_initial = 0, 0, 0

function on_init(side)
    overwrite_pose(x_initial, y_initial, theta_initial)
end

function move(x, y, t)
	set_pose(x, y, t, false)
    sleep(0.2)
	while not is_motion_done() do
		sleep(0.2)
	end
end

function on_run()
    move(500, 0, 0)
end

function resume_loop()
    return 100
end
