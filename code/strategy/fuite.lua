pi_half = 1.57079
x_initial, y_initial, theta_initial = 1150, 120, 1.57
score = 0

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
    score = score + 20
    print("score  " .. score)
    sleep(1.0)
    move(x_initial, 500, theta_initial) -- translation perpendi au mur
    move(1500, 800, 1.57) -- milieu
    move(1500, 1300, 3.14) -- milieu bis
        --TODO : attrapage
    --move(1100, 1300, -1.57) -- prépa attrapage

    move(400, 1400, theta_initial)
end

function on_end()
    score = score + 10
    print("score  " .. score)
    move(400, 1550, theta_initial) --atteinte zone arrivée
end

function resume_loop()
    return 100
end
