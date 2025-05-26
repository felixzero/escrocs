pi_half = 1.57079
x_initial, y_initial, theta_initial = 1200, 120, 1.57
score = 0

function on_init(side)
    overwrite_pose(x_initial, y_initial, theta_initial)
    --move(x_initial, y_initial+100, theta_initial)
    move_servo(0, 8500)
    move_servo(1, 8000)
    sleep(1.0)
    move_stepper(2, -200, 0.15)
end

function move(x, y, t, is_avoidance)
    avoidance = is_avoidance or true
	set_pose(x, y, t, avoidance)
    sleep(0.2)
	while not is_motion_done() do
		sleep(0.2)
	end
end

function move_until_button(x,y,t, is_avoidance, channel_to_monitor, iteration_wait)
    avoidance = is_avoidance or true
    set_pose(x,y,t, avoidance)
    iteration = 0
    while get_button(107) == 1 or iteration > iteration_wait do
        iteration = iteration + 1
        sleep(0.02)
    end
    stop_motion()
end


function on_run()
    move_stepper(2, 1600, 0.15) --preparation choppage planche
    --A cause de pas de banderole :
    sleep(5.0)
    score = score + 20
    print("score" .. score)

    -- deplacement vers premiere ensemble
    set_pose(x_initial, 300, theta_initial, false) -- translation perpendi au mur
    local iter = 0
    while get_button(25) == false and iter < 150 do
        sleep(0.02)
        iter = iter + 1
    end
    stop_motion()
    sleep(0.1)
    move_servo(1, 2000) --deploiement bras ultraetendu
    move_stepper(2, -200, 0.15) --attrape  2 planches
    sleep(3.0)
    move_servo(1, 8000) --retractage bras
    sleep(0.5)
    move_stepper(2, 625, 0.15) -- transport planches
    sleep(2.0)

    move(x_initial, 450, theta_initial, false)


    -- construction
    move_servo(1, 2000) --deploiement bras
    sleep(0.7)
    move_stepper(2, 1000, 0.15) -- attrapage 1 planche
    sleep(1.5)
    move_servo(1, 8000) --retractage bras
    sleep(0.5)
    move_stepper(2, 1500, 0.15) -- attrapage 1 planche
    sleep(1.5)
    move(x_initial, 350, theta_initial, false) -- reculage
    move_stepper(2, 750, 0.15) -- Depose de haut de 1 planche
    sleep(2.0)
    move_servo(1, 2500) --deploiement bras
    move_stepper(2, 950, 0.15) -- position haute 1 planche
    sleep(1.0)
    move_servo(1, 8000) --retractage bras
    move(x_initial, 250, theta_initial, false) -- translation perpendi au mur
    score = score + 8
    print(score)

    -- go en zone attente
    --move(780, 500, -1.57, true) -- translation
    --move(400, 1400, -1.57, true) -- atteinte zone attente, bras vers l'avant
--

end

function resume_loop()
    return 100
end

function on_end()
    move(x_initial, 600, theta_initial, false)

end
