pi_half = 1.57079
x_initial, y_initial, theta_initial = 1200, 120, 1.57

function on_init(side)
    overwrite_pose(x_initial, y_initial, theta_initial)
    move_without_avoidance(x_initial, y_initial+100, theta_initial)
    move_servo(0, 8500)
end

function move(x, y, t)
	set_pose(x, y, t, true)
    sleep(0.2)
	while not is_motion_done() do
		sleep(0.2)
	end
end

function move_without_avoidance(x, y, t)
	set_pose(x, y, t, false)
    sleep(0.2)
	while not is_motion_done() do
		sleep(0.2)
	end
end
function on_run()
    move_without_avoidance(x_initial, y_initial, theta_initial)
    overwrite_pose(x_initial, y_initial, theta_initial)
    move_servo(0, 6000)
    sleep(0.2)
    move_servo(0, 4000)
    sleep(0.2)
    move_servo(0, 2000)
    sleep(1.0)
    print("score  20")
    move(x_initial, 500, theta_initial) -- translation perpendi au mur
    move(x_initial, 500, 3.14)
    move(400, 500, 3.14) -- translation parallele au mur
    move(400, 500, theta_initial)
    move(400, 1400, theta_initial)
end

function on_end()
    print("score  30")
    move(400, 1550, theta_initial) --atteinte zone arrivée
end

function resume_loop()
    return 100
end
