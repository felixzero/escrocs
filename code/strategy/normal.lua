pi_half = 1.57079
x_initial, y_initial, theta_initial = 1150, 120, 1.57
score = 0

function on_init(side)
    overwrite_pose(x_initial, y_initial, theta_initial)
    move_without_avoidance(x_initial, y_initial+100, theta_initial)
    move_servo(0, 8500)
    move_stepper(2, 1600, 0.15) --preparation choppage planche
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

    score = score + 20
    print("score  " .. score)
    
    move(x_initial, 500, theta_initial) -- translation perpendi au mur
    move(x_initial, 500, 3.14)
    move(775, 500, -1.57) -- translation parallele au mur & mise en place planche

    -- deplacement vers premiere ensemble
    move_without_avoidance(750, 410, -1.57) -- translation perpendi au mur
    move_servo(1, 2000) --deploiement bras ultraetendu
    move_stepper(2, -200, 0.15) --attrape  2 planches
    sleep(3.0)
    move_servo(1, 8000) --retractage bras
    sleep(0.5)
    move_stepper(2, 625, 0.15) -- transport planches
    sleep(2.0)
    
    move_without_avoidance(775, 220, -1.57) -- translation jusqu'au mur
    
    -- construction
    move_servo(1, 2000) --deploiement bras
    sleep(0.7)
    move_stepper(2, 1000, 0.15) -- attrapage 1 planche
    sleep(1.5)
    move_servo(1, 8000) --retractage bras
    sleep(0.5)
    move_stepper(2, 1300, 0.15) -- attrapage 1 planche
    sleep(1.5)
    move(775, 340, -1.57) -- reculage
    move_stepper(2, 750, 0.15) -- Depose de haut de 1 planche
    sleep(2.0)
    move_servo(1, 2500) --deploiement bras
    move_stepper(2, 950, 0.15) -- position haute 1 planche
    sleep(1.0)
    move_servo(1, 8000) --retractage bras
    score = score + 8
    print("score  " .. score)
    move(775, 500, 3.14) -- translation perpendi au mur


    -- deplacemetn vers deuxieme ensemble


    -- zone arrivée
    move(700, 1300, 3.14)
    move(400, 1400, theta_initial)
end





function on_end()
    move(400, 1550, theta_initial) --atteinte zone arrivée
    score = score + 10
    print("score  " .. score)
end

function resume_loop()
    return 100
end
